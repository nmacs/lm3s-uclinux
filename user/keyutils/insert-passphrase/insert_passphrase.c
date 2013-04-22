#include <stdio.h>
#include <ecryptfs.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <keyutils.h>

//#define DEBUG

#ifdef DEBUG
void usage(void)
{
	printf("Usage:\n"
	       "\n"
	       "insert-passphrase file\n");
}
#else
#  define usage() do {} while(0)
#endif

static void from_hex(char *dst, char *src, int dst_size)
{
        int x;
        char tmp[3] = { 0, };

        for (x = 0; x < dst_size; x++) {
                tmp[0] = src[x * 2];
                tmp[1] = src[x * 2 + 1];
                dst[x] = (char)strtol(tmp, NULL, 16);
        }
}

/**
 * @return Zero on success
 */
static int generate_payload(struct ecryptfs_auth_tok *auth_tok, const char *passphrase_sig,
		 char *salt, char *session_key_encryption_key)
{
	int rc = 0;
	int major, minor;

	memset(auth_tok, 0, sizeof(struct ecryptfs_auth_tok));
	//ecryptfs_get_versions(&major, &minor, NULL);
	major = 0;
	minor = 4;
	auth_tok->version = (((uint16_t)(major << 8) & 0xFF00)
			     | ((uint16_t)minor & 0x00FF));
	auth_tok->token_type = ECRYPTFS_PASSWORD;
	strncpy((char *)auth_tok->token.password.signature, passphrase_sig,
		ECRYPTFS_PASSWORD_SIG_SIZE);
	memcpy(auth_tok->token.password.salt, salt, ECRYPTFS_SALT_SIZE);
	memcpy(auth_tok->token.password.session_key_encryption_key,
	       session_key_encryption_key, ECRYPTFS_MAX_KEY_BYTES);
	/* TODO: Make the hash parameterizable via policy */
	auth_tok->token.password.session_key_encryption_key_bytes =
		ECRYPTFS_MAX_KEY_BYTES;
	auth_tok->token.password.flags |=
		ECRYPTFS_SESSION_KEY_ENCRYPTION_KEY_SET;
	/* The kernel code will encrypt the session key. */
	auth_tok->session_key.encrypted_key[0] = 0;
	auth_tok->session_key.encrypted_key_size = 0;
	/* Default; subject to change by kernel eCryptfs */
	auth_tok->token.password.hash_algo = PGP_DIGEST_ALGO_SHA512;
	auth_tok->token.password.flags &= ~(ECRYPTFS_PERSISTENT_PASSWORD);
	return rc;
}

/**
 * @auth_tok: (out) This function will allocate; callee must free
 * @auth_tok_sig: (out) Allocated memory this function fills in:
                        (ECRYPTFS_SIG_SIZE_HEX + 1)
 * @fekek: (out) Allocated memory this function fills in: ECRYPTFS_MAX_KEY_BYTES
 * @salt: (in) salt: ECRYPTFS_SALT_SIZE
 * @passphrase: (in) passphrase: ECRYPTFS_MAX_PASSPHRASE_BYTES
 */
static int ecryptfs_generate_passphrase_auth_tok(struct ecryptfs_auth_tok **auth_tok,
					  const char *auth_tok_sig, char *fekek,
					  char *salt, char *passphrase)
{
	int rc;

	memcpy(fekek, passphrase, ECRYPTFS_MAX_KEY_BYTES);

	*auth_tok = malloc(sizeof(struct ecryptfs_auth_tok));
	if (!*auth_tok) {
#ifdef DEBUG
		printf("Unable to allocate memory for auth_tok\n");
#endif
		rc = -ENOMEM;
		goto out;
	}
	rc = generate_payload(*auth_tok, auth_tok_sig, salt, fekek);
	if (rc) {
#ifdef DEBUG
		printf("Error generating payload for auth tok key; "
		       "rc = [%d]\n", rc);
#endif
		rc = (rc < 0) ? rc : rc * -1;
		goto out;
	}
out:
	return rc;
}

static int ecryptfs_add_auth_tok_to_keyring(struct ecryptfs_auth_tok *auth_tok,
				     const char *auth_tok_sig)
{
	int rc;

	rc = (int)keyctl_search(KEY_SPEC_USER_KEYRING, "user", auth_tok_sig, 0);
	if (rc != -1) { /* we already have this key in keyring; we're done */
		rc = 1;
		goto out;
	} else if ((rc == -1) && (errno != ENOKEY)) {
		int errnum = errno;

#ifdef DEBUG
		printf("keyctl_search failed: %m errno=[%d]\n",
		       errnum);
#endif
		rc = (errnum < 0) ? errnum : errnum * -1;
		goto out;
	}
	rc = add_key("user", auth_tok_sig, (void *)auth_tok,
		     sizeof(struct ecryptfs_auth_tok), KEY_SPEC_USER_KEYRING);
	if (rc == -1) {
		rc = -errno;
#ifdef DEBUG
		printf("Error adding key with sig [%s]; rc = [%d] "
		       "\"%m\"\n", auth_tok_sig, rc);
		if (rc == -EDQUOT)
			printf("Error adding key to keyring - keyring is full\n");
#endif
		goto out;
	}
	rc = 0;
out:
	return rc;
}

/**
 * This is the common functionality used to put a password generated key into
 * the keyring, shared by both non-interactive and interactive signature
 * generation code.
 *
 * Returns 0 on add, 1 on pre-existed, negative on failure.
 */
static int ecryptfs_add_passphrase_key_to_keyring(const char *auth_tok_sig, char *passphrase,
					   char *salt)
{
	int rc;
	char fekek[ECRYPTFS_MAX_KEY_BYTES];
	struct ecryptfs_auth_tok *auth_tok = NULL;

	rc = ecryptfs_generate_passphrase_auth_tok(&auth_tok, auth_tok_sig,
						   fekek, salt, passphrase);
	if (rc) {
#ifdef DEBUG
		printf("%s: Error attempting to generate the "
		       "passphrase auth tok payload; rc = [%d]\n",
		       __FUNCTION__, rc);
#endif
		goto out;
	}
	rc = ecryptfs_add_auth_tok_to_keyring(auth_tok, auth_tok_sig);
	if (rc < 0) {
#ifdef DEBUG
		printf("%s: Error adding auth tok with sig [%s] to "
		       "the keyring; rc = [%d]\n", __FUNCTION__, auth_tok_sig,
		       rc);
#endif
		goto out;
	}
out:
	if (auth_tok) {
		memset(auth_tok, 0, sizeof(*auth_tok));
		free(auth_tok);
	}
	return rc;
}

static int read_passphrase_from_file(char* passphrase, char* passphrase_sig, char* filename)
{
	int fd;
	int size;
	int rc = 0;
	char hex_passphrase[ECRYPTFS_MAX_KEY_BYTES * 2 + ECRYPTFS_PASSWORD_SIG_SIZE];

	if ((fd = open(filename, O_RDONLY)) == -1) {
#ifdef DEBUG
		printf("Error attempting to open [%s] for reading\n", filename);
#endif
		rc = -EIO;
		goto out;
	}

	if ((size = read(fd, hex_passphrase, sizeof(hex_passphrase))) != sizeof(hex_passphrase)) {
#ifdef DEBUG
		printf("Error attempting to read "
		       "passphrase from file [%s]; size = [%zu]\n",
		       filename, size);
#endif
		rc = -EIO;
		close(fd);
		goto out;
	}
	close(fd);

	from_hex(passphrase, hex_passphrase + ECRYPTFS_SIG_SIZE_HEX, ECRYPTFS_MAX_KEY_BYTES);
	memcpy(passphrase_sig, hex_passphrase, ECRYPTFS_SIG_SIZE_HEX);
	passphrase_sig[ECRYPTFS_SIG_SIZE_HEX] = '\0';

out:
	return rc;
}

/**
 * ecryptfs_insert_wrapped_passphrase_into_keyring()
 *
 * Inserts two auth_tok objects into the user session keyring: a
 * wrapping passphrase auth_tok and the unwrapped passphrase auth_tok.
 *
 * Returns the signature of the wrapped passphrase that is inserted
 * into the user session keyring.
 */
static int ecryptfs_insert_passphrase_into_keyring(char *passphrase_sig, char *filename, char *salt)
{
	char decrypted_passphrase[ECRYPTFS_MAX_PASSPHRASE_BYTES + 1] ;
	int rc = 0;

	if ((rc = read_passphrase_from_file(decrypted_passphrase, passphrase_sig, filename))) {
#ifdef DEBUG
		printf("Error attempting to read passphrase from "
		       "file [%s]; rc = [%d]\n", filename, rc);
#endif
		rc = -EIO;
		goto out;
	}

	if ((rc = ecryptfs_add_passphrase_key_to_keyring(passphrase_sig,
							 decrypted_passphrase,
							 salt)) < 0) {
#ifdef DEBUG
		printf("Error attempting to add passphrase key to "
		       "user session keyring; rc = [%d]\n", rc);
#endif
	}

out:
	return rc;
}

int main(int argc, char *argv[])
{
	char *file;
	char salt[ECRYPTFS_SALT_SIZE];
	char passphrase_sig[ECRYPTFS_SIG_SIZE_HEX];
	int rc = 0;

	 if (argc == 2) {
		file = argv[1];
	} else {
		usage();
		goto out;
	}

	from_hex(salt, ECRYPTFS_DEFAULT_SALT_HEX, ECRYPTFS_SALT_SIZE);

	if ((rc = ecryptfs_insert_passphrase_into_keyring(passphrase_sig, file, salt)) < 0) {
#ifdef DEBUG
		fprintf(stderr, "%s [%d]\n",
			ECRYPTFS_ERROR_UNWRAP_AND_INSERT, rc);
		fprintf(stderr, "%s\n", ECRYPTFS_INFO_CHECK_LOG);
#endif
		rc = 1;
		goto out;
	} else
		rc = 0;
#ifdef DEBUG
	printf("Inserted key with sig [%s] into the user session "
	       "keyring\n", passphrase_sig);
#endif
out:
	return rc;
}
