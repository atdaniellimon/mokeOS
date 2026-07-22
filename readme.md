<div align="center">
  <h1>Galileo</h1>
  <p><strong>A bespoke operating system engineered for elegance and performance.</strong></p>
  <br>
</div>

Galileo OS is a proprietary operating system built from the ground up to explore new paradigms in desktop computing. By stripping away legacy abstractions, Galileo delivers a fluid, responsive, and visually stunning user experience straight from the boot screen.

---

## Features

- **Direct Framebuffer Architecture**  
  Galileo bypasses heavy traditional display servers (like X11 or Wayland). Its custom `Picasso` compositor communicates directly with the hardware framebuffer, ensuring ultra-low latency and immediate pixel delivery.

- **PicassoSDK Toolkit**  
  At the heart of the system is *Picasso*, a custom-built GUI engine. It renders complex layouts, real-time background blur algorithms, and deep alpha-blended shadows natively, without requiring external compositing managers.

- **Subpixel Typography**  
  Text rendering is treated as a first-class citizen. Galileo uses integrated subpixel anti-aliasing to deliver crisp, beautiful typography across the entire interface.

- **Self-Contained Ecosystem**  
  Applications in Galileo are distributed as `.gapp` bundles. These self-contained packages house the executable, assets, and metadata together, keeping the filesystem clean and modular.

---

## How do I boot it?

Galileo OS is distributed exclusively as a pre-compiled, bootable ISO image. 

### Running on Virtual Machines
For the best experience, we recommend booting the ISO using **QEMU** or **VirtualBox** with UEFI support enabled. 

**Quick start with QEMU:**
```bash
qemu-system-x86_64 -m 2G -bios /path/to/OVMF.fd -cdrom galileo.iso -vga virtio
```

### Running on Bare Metal
1. Flash the `galileo.iso` image to a USB drive using a tool like [Rufus](https://rufus.ie/) or `dd`.
2. Boot your machine in UEFI mode.
3. Enjoy the Galileo experience directly from the live environment.

---

<div align="center">
  <sub>Built with precision. Designed by Daniel Limón</sub>
</div>
