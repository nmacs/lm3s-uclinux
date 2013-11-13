/*
 * cfg
 */

//usage:#define cfg_trivial_usage ""
//usage:#define cfg_full_usage ""

#include "libbb.h"
#include <stdio.h>

#include "j0g.h"
#include "js0n.h"

#define CONFIG_PATH "/etc/config/config"
#define DEFAULT_CONFIG_PATH "/etc/default_config/config"
#define INDEX_SIZE 128
#define OBJECT_SIZE (INDEX_SIZE/4)

#undef DEBUG

#ifdef DEBUG
# define dprint(...) printf(__VA_ARGS__)
#else
# define dprint(...)
#endif

struct json_item
{
   char *text;
   size_t size;
};

struct cfg_section;
struct cfg_config;
struct configuration;

struct cfg_option
{
   struct json_item name;
   struct json_item value;
   struct cfg_section *section;
};

struct cfg_section
{
   struct json_item name;
   struct cfg_option *options;
   size_t noptions;
   struct cfg_config *config;
};

struct cfg_config
{
   struct json_item name;
   struct cfg_section *sections;
   size_t nsections;
   struct configuration *configuration;
};

struct configuration
{
   struct cfg_config *configs;
   size_t nconfigs;
   char *version;
   char *data;
};

static char* load_file(size_t *len, const char* path)
{
	dprint("load_file 1\n");
   char *content = 0;
   long size, size_f;
   FILE *f = 0;
   f = fopen(path, "r");
   if (!f)
      goto err;
	 dprint("load_file 2\n");
   if (fseek(f, 0L, SEEK_END) != 0)
      goto err;
   size = ftell(f);
   if (size <= 0)
      goto err;
	 dprint("load_file 3\n");
   content = (char*)malloc(size);
   if (!content)
      goto err;
	 dprint("load_file 4\n");
   if (fseek(f, 0L, SEEK_SET) != 0)
      goto err;
	dprint("load_file 5\n");
   if (fread(content, sizeof(char), size, f) != (size_t)size)
      goto err;
dprint("load_file 6\n");
   fclose(f);
   *len = (size_t)size;
   return content;

err:
dprint("load_file 7\n");
   if (f)
      fclose(f);
   if (content)
      free(content);
	 
	 dprint("load_file 8\n");

   return 0;
}

static int load_option(struct cfg_option *option, char *data, size_t data_size)
{
   unsigned short index[INDEX_SIZE];
   int value_i;
	 
	 dprint("load_option 1 data:%.*s\n", (int)data_size, data);

   memset(option, 0, sizeof(*option));
   memset(index, 0, sizeof(index));

   if (js0n(data, data_size, index, INDEX_SIZE))
      return -1;

   if ((value_i = j0g_val("value", data, index)) == 0)
      return -1;

   option->value.text = data + index[value_i];
   option->value.size = index[value_i + 1];

   return 0;
}

static int load_section(struct cfg_section *section, char *data, size_t data_size)
{
   int i, item, options_i;
   unsigned short index[INDEX_SIZE];
	 unsigned short options_index[INDEX_SIZE];
   size_t noptions = 0;
   struct cfg_option *options = 0;
   char *options_data = 0;

   memset(section, 0, sizeof(*section));
	 
	 dprint("load_section 1 data:%.*s\n", (int)data_size, data);

   memset(index, 0, sizeof(index));
   if (js0n(data, data_size, index, INDEX_SIZE))
      goto err;

   options_i = j0g_val("options", data, index);
   if (!options_i)
      goto err;

   memset(options_index, 0, sizeof(options_index));
   options_data = data + index[options_i];
   if (options_data[0] != '{' || js0n(options_data, index[options_i + 1], options_index, INDEX_SIZE))
      goto err;

	 dprint("load_section 2\n");

   for (i = 0; options_index[i]; i += 4)
      noptions++;

   if (noptions) {
      options = (struct cfg_option*)malloc(noptions * sizeof(struct cfg_option));
      if (!options)
         goto err;
      memset(options, 0, noptions * sizeof(struct cfg_option));

			dprint("load_section 3\n");

      for (i = 0, item = 0; item < noptions; i += 4, item++) {
         struct cfg_option *option = options + item;
         char *option_data = options_data + options_index[i + 2];
         size_t option_size = options_index[i + 3];
				 dprint("load_section 4\n");
         if (load_option(option, option_data, option_size))
            goto err;
				 dprint("load_section 5\n");
         option->name.text = options_data + options_index[i];
         option->name.size = options_index[i + 1];
         option->section = section;
      }
      section->options = options;
   }
   section->noptions = noptions;

   return 0;

err:
   if (options) {
      free(options);
   }
   return -1;
}

static void free_section(struct cfg_section *section)
{
   int i;
   if (section->options) {
      free(section->options);
   }
}

static int load_config(struct cfg_config *cfg, char *data, size_t data_size)
{
   int i, item, sections_i;
   unsigned short index[INDEX_SIZE];
   unsigned short sections_index[INDEX_SIZE];
   size_t nsections = 0;
   struct cfg_section *sections = 0;
   char *sections_data = 0;
	 
	 dprint("load_config 1 data:%.*s\n", (int)data_size, data);

   memset(index, 0, sizeof(index));
   if (js0n(data, data_size, index, INDEX_SIZE))
      goto err;

   sections_i = j0g_val("sections", data, index);
   if (!sections_i)
      goto err;

	 memset(sections_index, 0, sizeof(sections_index));
	 sections_data = data + index[sections_i];
	 if (sections_data[0] != '{' || js0n(sections_data, index[sections_i + 1], sections_index, INDEX_SIZE))
      goto err;

   dprint("load_config 2\n");

   memset(cfg, 0, sizeof(cfg));

   for (i = 0; sections_index[i]; i += 4)
      nsections++;

   if (nsections) {
      sections = (struct cfg_section*)malloc(nsections * sizeof(struct cfg_section));
      if (!sections)
         goto err;
      memset(sections, 0, nsections * sizeof(struct cfg_section));

			dprint("load_config 3 nsections:%u\n", nsections);

      for (i = 0, item = 0; item < nsections; i += 4, item++) {
         struct cfg_section *section = sections + item;
         char *section_data = sections_data + sections_index[i + 2];
         size_t section_size = sections_index[i + 3];
				 dprint("load_config 4\n");
         if (load_section(section, section_data, section_size))
            goto err;
				 dprint("load_config 5\n");
         section->name.text = sections_data + sections_index[i];
         section->name.size = sections_index[i + 1];
         section->config = cfg;
      }
      cfg->sections = sections;
   }
   cfg->nsections = nsections;

   return 0;

err:
   if (sections) {
      for (i = 0; i < nsections; i++)
         free_section(sections + i);
      free(sections);
   }
   return -1;
}

static void free_config(struct cfg_config *config)
{
   int i;
   if (config->sections) {
      for (i = 0; i < config->nsections; i++)
         free_section(config->sections + i);
      free(config->sections);
   }
}

static int load_configuration(struct configuration *cfg, char *path, const char *filter)
{
   size_t data_size = 0;
   int i, item;
   unsigned short index[INDEX_SIZE];
   unsigned short configs_index[INDEX_SIZE];
   char *configs_data = 0;
   struct cfg_config *configs = 0;
   size_t nconfigs = 0;
	 
	 dprint("load_configuration 1 path:%s, filter:%s\n", path, filter);

   char *data = load_file(&data_size, path);
   if (!data)
      return -1;
	 
	 dprint("load_configuration 2 data:%.*s\n", (int)data_size, data);

   memset(index, 0, sizeof(index));
   if (data[0] != '{' || js0n(data, data_size, index, INDEX_SIZE)) {
      goto err;
   }
   
   dprint("load_configuration 3\n");

   int configs_i = j0g_val("configs", data, index);
   if (!configs_i)
      goto err;
   
   dprint("load_configuration 4 configs_i:%i\n", configs_i);

   memset(configs_index, 0, sizeof(configs_index));
   configs_data = data + index[configs_i];
   if (configs_data[0] != '{' || js0n(configs_data, index[configs_i + 1], configs_index, INDEX_SIZE))
      goto err;
   
   dprint("load_configuration 5\n");

   for (i = 0; configs_index[i]; i += 4) {
      char *config_name = configs_data + configs_index[i];
      size_t config_name_size = configs_index[i+1];
      if (!filter || (config_name_size == strlen(filter) && !strncmp(config_name, filter, config_name_size)))
         nconfigs++;
   }
   
   dprint("load_configuration 6 nconfigs:%u\n", nconfigs);

   if (nconfigs) {
      configs = (struct cfg_config*)malloc(nconfigs * sizeof(struct cfg_config));
      if (!configs)
         goto err;
      memset(configs, 0, nconfigs * sizeof(struct cfg_config));
			
			dprint("load_configuration 7\n");

      for (i = 0, item = 0; item < nconfigs; i += 4) {
         struct cfg_config *config = configs + item;
         char *config_name = configs_data + configs_index[i];
         size_t config_name_size = configs_index[i+1];
         char *config_data = configs_data + configs_index[i+2];
         size_t config_size = configs_index[i + 3];
				 dprint("load_configuration 8 config_name:%.*s\n", (int)config_name_size, config_name);
         if (!filter || (config_name_size == strlen(filter) && !strncmp(config_name, filter, config_name_size))) {
            if (load_config(config, config_data, config_size))
               goto err;
            config->name.text = config_name;
            config->name.size = config_name_size;
            config->configuration = cfg;
            item++;
						dprint("load_configuration 9\n");
         }
      }
   }

   cfg->version = j0g_str("version", data, index);
   if (!cfg->version)
      cfg->version = "0.0";

   cfg->data = data;
   cfg->configs = configs;
   cfg->nconfigs = nconfigs;

   return 0;

err:
   if (data)
      free(data);
   if (configs) {
      for (i = 0; i < nconfigs; i++)
         if (configs[i].name.text)
            free_config(configs + i);
      free(configs);
   }

   return -1;
}

static void free_configuration(struct configuration* cfg)
{
   if (cfg->data) {
      free(cfg->data);
      cfg->data = 0;
   }

   if (cfg->configs) {
      int i;
      for (i = 0; i < cfg->nconfigs; i++)
         if (cfg->configs[i].name.text)
            free_config(cfg->configs + i);
      free(cfg->configs);
   }
}

static int jsoncmp(struct json_item *a, struct json_item *b)
{
   return a->size == b->size ? strncmp(a->text, b->text, a->size) : a->size - b->size;
}

static void print_option(struct cfg_option *option)
{
   char *config_name = option->section->config->name.text;
   size_t config_name_size = option->section->config->name.size;
   char *section_name = option->section->name.text;
   size_t section_name_size = option->section->name.size;
   char *option_name = option->name.text;
   size_t option_name_size = option->name.size;
   char *option_value = option->value.text;
   size_t option_value_size = option->value.size;

   printf("%.*s_%.*s_%.*s=\"%.*s\"\n",
         (int)config_name_size,  config_name,
         (int)section_name_size, section_name,
         (int)option_name_size,  option_name,
         (int)option_value_size, option_value);
}

static void merge_sections(struct cfg_section *section, struct cfg_section *default_section)
{
   int i, j;

   for (i = 0; i < default_section->noptions; i++) {
      int found = 0;
      for (j = 0; j < section->noptions; j++)
         if (!jsoncmp(&default_section->options[i].name, &section->options[j].name)) {
            found = 1;
            break;
         }
      if (found)
         print_option(section->options + j);
      else
         print_option(default_section->options + i);
   }

   for (i = 0; i < section->noptions; i++) {
      int found = 0;
      for (j = 0; j < default_section->noptions; j++)
         if (!jsoncmp(&section->options[i].name, &default_section->options[j].name)) {
            found = 1;
            break;
         }
      if (!found)
         print_option(section->options + i);
   }
}

static void print_section(struct cfg_section *section)
{
   int i;
   char *config_name = section->config->name.text;
   size_t config_name_size = section->config->name.size;
   char *section_name = section->name.text;
   size_t section_name_size = section->name.size;
   printf("%.*s_%.*s=\"1\"\n",
         (int)config_name_size, config_name,
         (int)section_name_size, section_name);
   for (i = 0; i < section->noptions; i++) {
      print_option(section->options + i);
   }
}

static void merge_configs(struct cfg_config *cfg, struct cfg_config *default_cfg)
{
   int i, j;

   for (i = 0; i < default_cfg->nsections; i++) {
      int found = 0;
      for (j = 0; j < cfg->nsections; j++)
         if (!jsoncmp(&default_cfg->sections[i].name, &cfg->sections[j].name)) {
            found = 1;
            break;
         }
      if (found)
         merge_sections(default_cfg->sections + i, cfg->sections + j);
      else
         print_section(default_cfg->sections + i);
   }

   for (i = 0; i < cfg->nsections; i++) {
      int found = 0;
      for (j = 0; j < default_cfg->nsections; j++)
         if (!jsoncmp(&cfg->sections[i].name, &default_cfg->sections[j].name)) {
            found = 1;
            break;
         }
      if (!found)
         print_section(cfg->sections + i);
   }
}

static void print_config(struct cfg_config *cfg)
{
   int i;
   for (i = 0; i < cfg->nsections; i++) {
      print_section(cfg->sections + i);
   }
}

static void merge_configurations(struct configuration *cfg, struct configuration *default_cfg)
{
   int i, j;

   for (i = 0; i < default_cfg->nconfigs; i++) {
      int found = 0;
      for (j = 0; j < cfg->nconfigs; j++)
         if (!jsoncmp(&default_cfg->configs[i].name, &cfg->configs[j].name)) {
            found = 1;
            break;
         }
      if (found)
         merge_configs(default_cfg->configs + i, cfg->configs + j);
      else
         print_config(default_cfg->configs + i);
   }

   for (i = 0; i < cfg->nconfigs; i++) {
      int found = 0;
      for (j = 0; j < default_cfg->nconfigs; j++)
         if (!jsoncmp(&cfg->configs[i].name, &default_cfg->configs[j].name)) {
            found = 1;
            break;
         }
      if (!found)
         print_config(cfg->configs + i);
   }
}

static void print_configuration(struct configuration *cfg)
{
   int i;
   for (i = 0; i < cfg->nconfigs; i++) {
      print_config(cfg->configs + i);
   }
}

int cfg_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int cfg_main(int argc, char **argv)
{
   struct configuration cfg;
   struct configuration default_cfg;
   char *filter = 0;

   memset(&cfg, 0, sizeof(cfg));
   memset(&default_cfg, 0, sizeof(default_cfg));

   if (argc >= 2)
      filter = argv[1];

   if (load_configuration(&default_cfg, DEFAULT_CONFIG_PATH, filter))
      return -1;

   if (!load_configuration(&cfg, CONFIG_PATH, filter)) {
      merge_configurations(&cfg, &default_cfg);
      free_configuration(&cfg);
   }
   else {
      print_configuration(&default_cfg);
   }

   free_configuration(&default_cfg);

   return 0;
}
