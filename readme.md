# Galileo OS

A bespoke, operating system built from the ground up to explore custom graphics pipelines and UI rendering paradigms. Galileo features its own dedicated window server, graphics primitives, and a declarative UI toolkit.

## Core Architecture

- **Picasso:** A custom, display server that communicates directly with the hardware framebuffer. It manages dirty-rectangle partial redraws, alpha blending, and window composition without relying on heavy abstractions like X11 or Wayland.
- **Picasso SDK:** A custom declarative GUI toolkit built in C. It uses a modern compound literal syntax allowing developers to define complex layouts, background blur filters, and state management in a single fluid block of code.

## Key Features

- **Direct Framebuffer Access:** Fast, direct-to-memory pixel pushing with custom software rendering fallbacks.
- **Advanced Rendering:** Real-time background blur algorithms and full alpha blending for overlapping windows.
- **Declarative Syntax:** Build interfaces rapidly using C99 inline structs without managing complex memory lifecycles manually.
- **Subpixel Typography:** Crisp, anti-aliased font rendering via integrated `stb_truetype`.
- **Custom App Bundles:** A bespoke `.gapp` application structure for bundling executables, icons, and metadata natively.
