import os
import subprocess
import time
import pandas as pd

# Define paths
input_dir = "ADM-2024-Assignment-3-data-TPCH-SF-1"
decoded_dir = "decoded_files"
os.makedirs(decoded_dir, exist_ok=True)

# Define compression techniques for each data type category
compression_rules = {
    "integer": ["bin", "rle", "dic", "for", "dif"],
    "string": ["rle", "dic"]
}

# Mapping of file suffixes to categories
data_type_categories = {
    "int8": "integer",
    "int16": "integer",
    "int32": "integer",
    "int64": "integer",
    "string": "string"
}

# Initialize list for storing results
results = []

def run_command(args, stdout=None):
    """Run a shell command and return its output."""
    with subprocess.Popen(args, stdout=stdout if stdout else subprocess.PIPE, stderr=subprocess.PIPE) as process:
        stdout_data, stderr_data = process.communicate()
    return stdout_data, stderr_data, process.returncode

def get_file_size(filepath):
    """Return the size of a file in bytes."""
    return os.path.getsize(filepath) if os.path.isfile(filepath) else None

def encode_and_decode(file, technique, dtype):
    """Encode and decode a file, then log results."""
    file_path = os.path.join(input_dir, file)
    encoded_file = f"{file_path}.{technique}"  # Encoded file is created directly in input folder
    decoded_file = os.path.join(decoded_dir, f"{file}.{technique}.csv")  # Decoded output path

    # Encoding
    start_time = time.time()
    _, _, code = run_command(["./program", "en", technique, dtype, file_path])
    encoding_time = time.time() - start_time
    if code != 0:
        print(f"Encoding failed for {file} with {technique}")
        return None

    # Check file sizes
    original_size = get_file_size(file_path)
    encoded_size = get_file_size(encoded_file)

    # Decoding
    start_time = time.time()
    with open(decoded_file, "w") as f_out:
        _, _, code = run_command(["./program", "de", technique, dtype, encoded_file], stdout=f_out)
    decoding_time = time.time() - start_time
    if code != 0:
        print(f"Decoding failed for {file} with {technique}")
        return None

    decoded_size = get_file_size(decoded_file)

    # Difference check
    diff_cmd = ["diff", file_path, decoded_file]
    _, _, diff_code = run_command(diff_cmd)
    difference = "Yes" if diff_code == 0 else "No"

    # Record results
    results.append({
        "File": file,
        "Technique": technique,
        "Data Type": dtype,
        "Original Size (bytes)": original_size,
        "Encoded Size (bytes)": encoded_size,
        "Decoded Size (bytes)": decoded_size,
        "Encoding Time (s)": encoding_time,
        "Decoding Time (s)": decoding_time,
        "No Difference": difference
    })

# Process each file in the input directory
for file in os.listdir(input_dir):
    if file.endswith(".csv"):
        # Dynamically determine the data type from the filename
        dtype = file.split("-")[-1].replace(".csv", "")
        
        # Check if dtype is known and get its category
        if dtype in data_type_categories:
            category = data_type_categories[dtype]
            applicable_techniques = compression_rules[category]
            
            # Run each applicable compression technique on the file
            for technique in applicable_techniques:
                encode_and_decode(file, technique, dtype)
        else:
            print(f"Unknown data type for file: {file}")

# Display results in a structured format
df = pd.DataFrame(results)
print(df)
