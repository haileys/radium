# Radium

Hobby OS.

## Memory Map

| Begin       | End (exclusive) | Purpose
| ----------- | --------------- | -------
| `0000_0000` | `0000_1000`     | Null page. Not mapped in.
| `0000_1000` | `0010_0000`     | Low memory. Untouched.
| `0010_0000` | *???*           | The kernel is loaded here by GRUB.
| *???*       | `0fff_e000`     | Kernel dynamic allocation area.
| `0fff_e000` | `0fff_f000`     | Kernel stack guard page. Not mapped in.
| `0fff_f000` | `1000_0000`     | Kernel stack.
| `1000_0000` | `ffc0_0000`     | User address space.
| `ffc0_0000` | `ffff_f000`     | Recursively mapped page tables
| `ffff_f000` | `1_0000_0000`   | Recursively mapped page directory
