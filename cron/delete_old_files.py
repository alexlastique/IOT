import os
import time
from datetime import datetime, timedelta

# Directory to clean
DIR_TO_CLEAN = "/home/iotg2/api-capteurs/photos/"

# Age threshold (2 hours)
THRESHOLD = timedelta(hours=2)

# Current time
now = datetime.now()

# Iterate through files
for filename in os.listdir(DIR_TO_CLEAN):
    file_path = os.path.join(DIR_TO_CLEAN, filename)
    
    if os.path.isfile(file_path):
        # Get file modification time
        mtime = datetime.fromtimestamp(os.path.getmtime(file_path))
        
        # Check if file is older than threshold
        if now - mtime > THRESHOLD:
            try:
                os.remove(file_path)
                print(f"Deleted: {file_path}")
            except Exception as e:
                print(f"Error deleting {file_path}: {e}")
