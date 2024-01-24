#!/bin/bash

# Security Check Script

# Output directory for logs
LOG_DIR="/var/log/security_checks"
mkdir -p "$LOG_DIR"

# Log file
LOG_FILE="$LOG_DIR/security_check_$(date +'%Y%m%d_%H%M%S').log"

# Function to log messages
log_message() {
    echo "$(date +'%Y-%m-%d %H:%M:%S') - $1" >> "$LOG_FILE"
}

# Check for root privileges
if [ "$EUID" -ne 0 ]; then
    log_message "Error: Please run this script as root."
    exit 1
fi

# Check for latest package updates
log_message "Checking for package updates..."
apt-get update
apt-get upgrade -y

# Check for open ports
log_message "Checking for open ports..."
netstat -tuln >> "$LOG_FILE"

# Check for listening services
log_message "Checking for listening services..."
ss -tuln >> "$LOG_FILE"

# Check for users with UID 0 (root)
log_message "Checking for users with UID 0 (root)..."
awk -F: '($3 == 0) { print $1 }' /etc/passwd >> "$LOG_FILE"

# Check for world-writable files
log_message "Checking for world-writable files..."
find / -type f -perm -o+w >> "$LOG_FILE"

# Check for setuid and setgid files
log_message "Checking for setuid and setgid files..."
find / -type f \( -perm -4000 -o -perm -2000 \) >> "$LOG_FILE"

# Add more checks as needed...

log_message "Security checks completed. Results logged to $LOG_FILE"
