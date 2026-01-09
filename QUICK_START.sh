#!/bin/bash
# Container Control System v2.0 - Quick Start Deployment

echo "════════════════════════════════════════════════════════"
echo "  Container Control System v2.0 - Deployment Helper"
echo "  Tri-Modal: Container/Vehicle/Maritime Detection"
echo "════════════════════════════════════════════════════════"
echo ""

# Check if we're in the right directory
if [ ! -d "firmware/application/external/container_control" ]; then
    echo "❌ ERROR: Run this script from mayhem-firmware root directory"
    echo "   cd ~/mayhem-firmware && ./QUICK_START.sh"
    exit 1
fi

echo "✅ Container Control files found"
echo ""

# Option selection
echo "Choose deployment method:"
echo ""
echo "1) GitHub Actions (Recommended for macOS ARM64)"
echo "   - Automatic build on GitHub servers"
echo "   - No local compilation needed"
echo "   - Downloads ready-to-use .bin file"
echo ""
echo "2) Local Docker Build (Linux x86_64 only)"
echo "   - Builds on your machine"
echo "   - Requires Docker"
echo "   - May fail on macOS ARM64"
echo ""
echo "3) Prepare SD Card (if you already have firmware)"
echo "   - Copy firmware to SD card"
echo "   - Flash to HackRF"
echo ""
echo "4) Show file summary and exit"
echo ""

read -p "Enter choice [1-4]: " choice

case $choice in
    1)
        echo ""
        echo "════════ GitHub Actions Setup ════════"
        echo ""
        echo "Step 1: Initialize Git repository"
        if [ ! -d ".git" ]; then
            git init
            echo "✅ Git initialized"
        else
            echo "✅ Git already initialized"
        fi

        echo ""
        echo "Step 2: Add all files"
        git add .
        echo "✅ Files staged"

        echo ""
        echo "Step 3: Create commit"
        git commit -m "Add Container Control System v2.0

Features:
- Container Mode (Shipping containers)
- Vehicle Mode (Border control, tracker detection)
- Maritime Mode (Ports, AIS/EPIRB, boat scanning)
- RX-Only operation with Driver Gate TX-blocking
- 10 device types (vehicles + maritime)
- 6 scan profiles optimized for each mode
" || echo "⚠️ No changes to commit (already committed)"

        echo ""
        echo "Step 4: Push to GitHub"
        echo ""
        echo "Choose GitHub setup method:"
        echo "  a) GitHub CLI (gh) - automatic"
        echo "  b) Manual setup"
        echo ""
        read -p "Enter choice [a/b]: " gh_choice

        if [ "$gh_choice" = "a" ]; then
            if command -v gh &> /dev/null; then
                read -p "Repository name [mayhem-container-control]: " repo_name
                repo_name=${repo_name:-mayhem-container-control}

                gh repo create $repo_name --public --source=. --remote=origin --push
                echo "✅ Repository created and pushed!"
            else
                echo "❌ GitHub CLI not found. Install: brew install gh"
                echo "   Or choose option 'b' for manual setup"
                exit 1
            fi
        else
            echo ""
            echo "Manual GitHub setup:"
            echo "1. Go to https://github.com/new"
            echo "2. Create repository: mayhem-container-control"
            echo "3. Run these commands:"
            echo ""
            echo "   git remote add origin https://github.com/YOUR-USERNAME/mayhem-container-control.git"
            echo "   git branch -M main"
            echo "   git push -u origin main"
            echo ""
            read -p "Press Enter when done..."
        fi

        echo ""
        echo "════════ Build Started ════════"
        echo ""
        echo "✅ GitHub Actions will now build your firmware automatically!"
        echo ""
        echo "Check build status:"
        echo "   https://github.com/YOUR-USERNAME/mayhem-container-control/actions"
        echo ""
        echo "Build takes approximately 15-20 minutes."
        echo ""
        echo "When complete:"
        echo "1. Go to Actions tab"
        echo "2. Click on successful build"
        echo "3. Download 'portapack-mayhem-firmware' artifact"
        echo "4. Extract ZIP file"
        echo "5. Run: ./QUICK_START.sh and choose option 3"
        ;;

    2)
        echo ""
        echo "════════ Local Docker Build ════════"
        echo ""
        read -p "⚠️  This may fail on macOS ARM64. Continue? [y/N]: " confirm

        if [ "$confirm" != "y" ]; then
            echo "Cancelled. Try option 1 (GitHub Actions) instead."
            exit 0
        fi

        echo ""
        echo "Building Docker image..."
        docker build -t portapack-dev -f dockerfile .

        echo ""
        echo "Building firmware..."
        docker run --rm -v $(pwd):/havocsrc portapack-dev \
            bash -c "cd /havocsrc && rm -rf build && mkdir build && cd build && cmake .. && make -j1 firmware"

        echo ""
        echo "Build complete! Check for binaries:"
        find build -name "*.bin" | head -10
        ;;

    3)
        echo ""
        echo "════════ SD Card Preparation ════════"
        echo ""

        # Find firmware file
        if [ -f "build/firmware/portapack-mayhem-firmware.bin" ]; then
            FIRMWARE="build/firmware/portapack-mayhem-firmware.bin"
        elif [ -f "portapack-mayhem-firmware.bin" ]; then
            FIRMWARE="portapack-mayhem-firmware.bin"
        else
            echo "❌ Firmware file not found!"
            echo ""
            read -p "Enter path to firmware .bin file: " FIRMWARE

            if [ ! -f "$FIRMWARE" ]; then
                echo "❌ File not found: $FIRMWARE"
                exit 1
            fi
        fi

        echo "✅ Found firmware: $FIRMWARE"
        echo ""

        # Find SD card
        echo "Available disks:"
        if [ "$(uname)" = "Darwin" ]; then
            diskutil list | grep -E "(disk[0-9]|PORTAPACK|FAT)"
            echo ""
            read -p "Enter SD card mount point [/Volumes/PORTAPACK]: " sd_mount
            sd_mount=${sd_mount:-/Volumes/PORTAPACK}
        else
            lsblk
            echo ""
            read -p "Enter SD card mount point [/media/PORTAPACK]: " sd_mount
            sd_mount=${sd_mount:-/media/PORTAPACK}
        fi

        if [ ! -d "$sd_mount" ]; then
            echo "❌ Mount point not found: $sd_mount"
            echo "   Mount SD card first!"
            exit 1
        fi

        echo ""
        echo "Copying firmware to SD card..."
        cp -v "$FIRMWARE" "$sd_mount/"

        echo ""
        echo "✅ Firmware copied!"
        echo ""
        echo "Next steps:"
        echo "1. Safely eject SD card"
        if [ "$(uname)" = "Darwin" ]; then
            echo "   diskutil eject $sd_mount"
        else
            echo "   sudo umount $sd_mount"
        fi
        echo ""
        echo "2. Insert SD card into PortaPack"
        echo "3. Power on HackRF"
        echo "4. Update firmware:"
        echo "   Settings → Firmware → Update from SD"
        echo "   Select: portapack-mayhem-firmware.bin"
        echo ""
        echo "5. Find app in menu:"
        echo "   RX Apps → Container Ctrl (red icon)"
        ;;

    4)
        echo ""
        echo "════════ File Summary ════════"
        echo ""
        echo "Implemented files:"
        echo ""
        find firmware/application/external/container_control -type f | sort
        echo ""
        echo "Total lines of code:"
        find firmware/application/external/container_control -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1
        echo ""
        echo "Documentation:"
        ls -lh firmware/application/external/container_control/*.md
        ls -lh DEPLOYMENT_GUIDE.md QUICK_START.sh
        echo ""
        echo "Build configuration:"
        ls -lh .github/workflows/build-container-control.yml
        ;;

    *)
        echo "❌ Invalid choice"
        exit 1
        ;;
esac

echo ""
echo "════════════════════════════════════════════════════════"
echo "  Container Control System v2.0 - Deployment Helper"
echo "════════════════════════════════════════════════════════"
