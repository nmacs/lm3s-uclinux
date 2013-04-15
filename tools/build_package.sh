#!/bin/bash

package=$1
repo=$2
codename=$3

version=`cat $package/debian/control | grep Version: | cut -d' ' -f 2`
name=`cat $package/debian/control | grep Package: | cut -d' ' -f 2`
arch=`cat $package/debian/control | grep Architecture: | cut -d' ' -f 2`
depends=`cat $package/debian/control | grep Depends: | cut -d' ' -f 2-`

echo "old depends $depends"
depends=(`echo $depends`)
for dep_package in ${depends[@]}
do
	dep_package=${dep_package//,/}
	package_version=`echo $dep_package | cut -s -d'(' -f 2`
	if [ -n "$package_version" ]; then
		new_depends="$new_depends$dep_package, "
	else
		info=`reprepro --basedir $repo list $codename $dep_package | grep $arch`
		package_version=`echo $info | cut -s -d' ' -f 3`
		echo "$name depends from $dep_package $package_version"
		new_depends="$new_depends$dep_package(=$package_version), "
	fi
done
new_depends=${new_depends:0:${#new_depends}-2}
echo "new depends $new_depends"

deb="${name}_${version}_${arch}.deb"
echo "Package name: $deb"

rm -rf "$package/.repo"
mkdir -p "$package/.repo"

repo_deb="$package/.repo/$name.deb"

load_package.sh $name "file://$repo" $codename $arch $repo_deb

mkdir -p "$package/.content/DEBIAN"
cp $package/debian/control $package/.content/DEBIAN/control
echo "Filename: $deb" >> $package/.content/DEBIAN/control

sed "s/Depends: .*/Depends: $new_depends/" $package/.content/DEBIAN/control > $package/.content/DEBIAN/control.new
mv $package/.content/DEBIAN/control.new $package/.content/DEBIAN/control

if [ -f "$repo_deb" ]; then
	echo "$name found in repository"
	mkdir -p "$package/.repo/.content"
	dpkg-deb -x "$repo_deb" "$package/.repo/.content"
	dpkg-deb -e "$repo_deb" "$package/.repo/.content/DEBIAN"
	chmod -R g+w "$package/.repo/.content/DEBIAN"

	target_version_revision=`cat $package/.repo/.content/DEBIAN/control | grep Version: | cut -d' ' -f 2`
	target_version=`echo $target_version_revision | cut -d- -f1`

	if [ "$version" != "$target_version" ]; then
		publish=1
	else
		sed "s/Version: .*/Version: $target_version_revision/" $package/.content/DEBIAN/control > $package/.content/DEBIAN/control.new
		mv $package/.content/DEBIAN/control.new $package/.content/DEBIAN/control
		repo_deb_sum=`tar --mtime='1970-01-01' -C $package/.repo -c .content | md5sum | cut -d' ' -f1`
		deb_sum=`tar --mtime='1970-01-01' -C $package -c .content | md5sum | cut -d' ' -f1`
		if [ "$deb_sum" != "$repo_deb_sum" ]; then
			echo "Different content"
			#exit 1
			publish=1
			target_revision=`echo $target_version_revision | cut -s -d- -f2`
			if [ -n "$target_revision" ]; then
				target_revision=`expr $target_revision + 1`
			else
				target_revision="1"
			fi
			version="${version}-${target_revision}"
			deb="${name}_${version}_${arch}.deb"
			sed "s/Version: .*/Version: $version/" $package/.content/DEBIAN/control > $package/.content/DEBIAN/control.new
			mv $package/.content/DEBIAN/control.new $package/.content/DEBIAN/control
		fi
	fi
else
	publish=1
fi

if [ -n "$publish" ]; then
	echo "Build and publish: $package/$deb"
	dpkg --build "$package/.content" "$deb"
	reprepro --basedir $repo includedeb $codename $package/$deb
	rm $deb
else
	echo "Skip $deb"
fi