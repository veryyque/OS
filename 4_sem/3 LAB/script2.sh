#!/bin/bash

ln file_operations create_dir
ln file_operations list_dir
ln file_operations remove_dir
ln file_operations create_file
ln file_operations read_file
ln file_operations remove_file
ln file_operations create_symlink
ln file_operations read_symlink
ln file_operations read_symlink_target
ln file_operations remove_symlink
ln file_operations create_hardlink
ln file_operations remove_hardlink
ln file_operations print_file_info
ln file_operations change_permissions

rm -rf newCatalog

echo
./create_dir newCatalog
echo
./create_file newCatalog/newFile.txt
echo
./list_dir newCatalog
echo
./read_file newCatalog/newFile.txt
echo
./print_file_info newCatalog/newFile.txt
echo
./change_permissions newCatalog/newFile.txt 600
echo
./print_file_info newCatalog/newFile.txt
echo
./remove_file newCatalog/newFile.txt
echo
./create_file newCatalog/linkTEST.txt
echo
./create_symlink newCatalog/linkTEST.txt newCatalog/symlink.txt
echo
./create_hardlink newCatalog/linkTEST.txt newCatalog/hardlink.txt
echo
./print_file_info newCatalog/linkTEST.txt
echo
./read_symlink newCatalog/symlink.txt
echo
./read_symlink_target newCatalog/symlink.txt
echo
./remove_symlink newCatalog/symlink.txt
echo
./remove_hardlink newCatalog/hardlink.txt
echo
./remove_file newCatalog/linkTEST.txt
echo
./remove_dir newCatalog