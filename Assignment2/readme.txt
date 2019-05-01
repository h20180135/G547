1) Firstly we will compile using the following command:

   sudo make all

2)  We insert the block driver module using:

   sudo insmod block_driver.ko

3)  Then we can view our partition list using:
    
    ls -l /dev/disk_on_ram*

4)   Then we'll run the following commands:
    
    dd if=/dev/disk_on_ram of=dor
    ls -l dor
    dd if=/dev/zero of=/dev/disk_on_ram1 count=1

5)  Then we can write into the disk_on_ram device using:
    sudo chmod 777 /dev/disk_on_ram
    cat > /dev/disk_on_ram1

6)  Finally,we can read the disk_on_ram content using:

    xxd /dev/disk_on_ram1 | less
