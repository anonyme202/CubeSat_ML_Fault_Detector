'''
This script generates a sample dataset for CubeSat fault prediction.
It simulates both normal conditions and specific, classified faults based on the PDF specification.

Fault Classes:
- 0: Normal
- 1: OBC - No heartbeat
- 2: OBC - Overcurrent
- 3: TTC - UART timeout
- 4: TTC - Data corruption (CRC mismatch)
'''
import pandas as pd
import numpy as np
import os

# Configuration
NUM_SAMPLES = 5000
DATA_DIR = "data"
DATA_FILE_PATH = os.path.join(DATA_DIR, "cubesat_data.csv")

# Ensure data directory exists
os.makedirs(DATA_DIR, exist_ok=True)

def generate_data():
    data = []
    fault_distribution = {1: 0.05, 2: 0.05, 3: 0.05, 4: 0.05} # 5% for each fault type
    normal_ratio = 1 - sum(fault_distribution.values())

    for i in range(NUM_SAMPLES):
        # Default to normal state
        record = {
            "bus_voltage": np.random.uniform(4.95, 5.05),
            "current_draw": np.random.uniform(0.4, 0.6),
            "mcu_core_temp": np.random.uniform(20, 50),
            "heartbeat_signal": 1,
            "uart_packets_received": np.random.randint(95, 100),
            "crc_error_count": np.random.randint(0, 2),
            "uart_timeout": 0,
            "fault": 0 # Normal class
        }

        data.append(record)

    # Introduce faults based on distribution
    sample_indices = np.arange(NUM_SAMPLES)
    np.random.shuffle(sample_indices)
    start_index = 0

    for fault_class, ratio in fault_distribution.items():
        num_faults = int(NUM_SAMPLES * ratio)
        fault_indices = sample_indices[start_index : start_index + num_faults]
        start_index += num_faults

        for idx in fault_indices:
            data[idx]["fault"] = fault_class
            if fault_class == 1: # No heartbeat
                data[idx]["heartbeat_signal"] = 0
            elif fault_class == 2: # Overcurrent
                data[idx]["current_draw"] = np.random.uniform(0.7, 1.0)
                data[idx]["bus_voltage"] = np.random.uniform(4.8, 4.95) # Slight voltage drop
            elif fault_class == 3: # UART timeout
                data[idx]["uart_timeout"] = 1
                data[idx]["uart_packets_received"] = 0
            elif fault_class == 4: # Data corruption
                data[idx]["crc_error_count"] = np.random.randint(5, 20)

    df = pd.DataFrame(data)

    # Calculate power consumption for all records
    df['power_consumption'] = df['bus_voltage'] * df['current_draw']

    # Reorder columns
    final_columns = [
        'bus_voltage', 'current_draw', 'power_consumption', 'mcu_core_temp', 'heartbeat_signal',
        'uart_packets_received', 'crc_error_count', 'uart_timeout', 'fault'
    ]
    df = df[final_columns]

    df.to_csv(DATA_FILE_PATH, index=False)
    print(f"Successfully generated {NUM_SAMPLES} samples and saved to {DATA_FILE_PATH}")

if __name__ == "__main__":
    generate_data()