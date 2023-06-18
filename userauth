#!/bin/bash

# Path to the PostgreSQL configuration file
CONFIG_FILE="/path/to/postgresql.conf"

# Path to the pg_hba.conf file
HBA_FILE="/path/to/pg_hba.conf"

# Username and desired authentication method
USERNAME="myuser"
AUTH_METHOD="md5"

# Modify the authentication method in the pg_hba.conf file
sed -i "s/^\(local\s\+all\s\+${USERNAME}\s\+\).*$/\1${AUTH_METHOD}/" "$HBA_FILE"

# Restart the PostgreSQL server
service postgresql restart
# Use "systemctl restart postgresql" instead if you're using systemd

echo "Authentication method for user '${USERNAME}' modified successfully."
