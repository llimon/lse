
cd  src || exit 1

# Generate files
for a in `ls ../statics/*.h`; do echo $a; perl ../split_compat.pl $a; done

sh ../build_lib.sh -build-lsecompat
sh ../build_lib.sh -build-lsenet

sh ../create_master_header.sh -build-lsecompat
sh ../create_master_header.sh -build-lsenet

# copy libraries
sudo mkdir -p /usr/local/lse/lib
sudo cp *.so /usr/local/lse/lib
sudo cp *.a /usr/local/lse/lib

# Primary target location
sudo mkdir -p /usr/local/lse/include/lse
sudo cp *.h /usr/local/lse/include/lse/

# Master umbrella header
sudo cp lsecompat.h /usr/local/lse/include/
sudo cp lsenet.h /usr/local/lse/include/

# Temporary transitional symlink for old scripts
sudo ln -s /usr/local/lse/include/lse /usr/local/lse/include/compat
