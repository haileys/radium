# Radium

Hobby OS.

## Memory Map

* `0000_0000` ... `0000_1000`: Null page. Not mapped in.

* `0000_1000` ... `0010_0000`: Low memory. Untouched.

* `0010_0000` ... **???**: The kernel is loaded here by GRUB.

* **???** ... `0fff_e000`: Kernel dynamic allocation area.

* `0000_1000` ... whatever lol: Kernel. Same for all processes.

* `0fff_e000` ... `0fff_f000`: Kernel stack guard page. Not mapped in.

* `0fff_f000` ... `1000_0000`: Kernel stack.

* `1000_0000` .. `ffff_ffff`: User address space.

