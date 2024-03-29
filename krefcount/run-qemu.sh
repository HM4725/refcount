sudo qemu-system-x86_64 \
    -enable-kvm \
    -cpu host,pcid=on,vmx=on \
    -smp sockets=2,cores=12,threads=1 \
    -m 48G \
    -nographic \
    -drive if=virtio,file=./disks/VM.img,format=qcow2 \
    -kernel ./linux/build/arch/x86_64/boot/bzImage \
    -append "root=/dev/vda1 console=ttyS0 rw nokaslr" \
    -drive if=virtio,file=./disks/empty.img,format=raw,cache=none \
    -net nic \
    -net user,hostfwd=tcp::10022-:22 \
    -gdb tcp::12345