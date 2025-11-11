
'''
This script demonstrates how to use the converted TensorFlow Lite model (.tflite)
for CubeSat fault prediction and visualizes the output.
'''
import numpy as np
import tensorflow as tf
import os
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
import pandas as pd # Used only for fitting the scaler

# Configuration
DATA_FILE_PATH = "data/cubesat_data.csv"
MODELS_DIR = "models"
IMAGES_DIR = "images"
TFLITE_MODEL_PATH = os.path.join(MODELS_DIR, "fault_model.tflite")

# Ensure images directory exists
os.makedirs(IMAGES_DIR, exist_ok=True)

# Define the labels for the output classes
FAULT_CLASSES = {
    0: "Normal",
    1: "OBC - No heartbeat",
    2: "OBC - Overcurrent",
    3: "TTC - UART timeout",
    4: "TTC - Data corruption"
}

def plot_prediction(probabilities, sample_name):
    class_names = list(FAULT_CLASSES.values())
    prediction_index = np.argmax(probabilities)

    plt.figure(figsize=(10, 6))
    bars = plt.bar(class_names, probabilities, color='skyblue')
    bars[prediction_index].set_color('royalblue')

    plt.ylabel('Confidence')
    plt.title(f'Prediction Confidence for {sample_name}')
    plt.xticks(rotation=45, ha="right")
    plt.ylim(0, 1)

    for bar in bars:
        yval = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2.0, yval + 0.02, f'{yval:.2%}', ha='center', va='bottom')

    plot_filename = os.path.join(IMAGES_DIR, f"prediction_{sample_name.replace(' ', '_')}.png")
    plt.tight_layout()
    plt.savefig(plot_filename)
    print(f"Prediction plot saved to {plot_filename}")

def predict_fault_tflite(sample_data, sample_name):
    if not os.path.exists(TFLITE_MODEL_PATH):
        print(f"Error: Model file not found at '{TFLITE_MODEL_PATH}'.")
        print("Please run train_model.py first.")
        return

    try:
        df = pd.read_csv(DATA_FILE_PATH)
        features = df.drop('fault', axis=1).values
        scaler = StandardScaler().fit(features)
    except FileNotFoundError:
        print(f"Error: Data file '{DATA_FILE_PATH}' not found to create scaler.")
        return

    feature_order = ['bus_voltage', 'current_draw', 'power_consumption', 'mcu_core_temp', 'heartbeat_signal', 'uart_packets_received', 'crc_error_count', 'uart_timeout']
    sample_array = np.array([[sample_data[key] for key in feature_order]], dtype=np.float32)
    scaled_sample = scaler.transform(sample_array)

    interpreter = tf.lite.Interpreter(model_path=TFLITE_MODEL_PATH)
    interpreter.allocate_tensors()

    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    interpreter.set_tensor(input_details[0]['index'], scaled_sample)
    interpreter.invoke()

    output_data = interpreter.get_tensor(output_details[0]['index'])[0]
    prediction_index = np.argmax(output_data)
    confidence = np.max(output_data)

    print(f"Prediction: {FAULT_CLASSES[prediction_index]} (Confidence: {confidence:.2%})")
    plot_prediction(output_data, sample_name)

if __name__ == "__main__":
    # Example 1: A sample with normal readings
    normal_sample = {
        "bus_voltage": 5.01,
        "current_draw": 0.51,
        "power_consumption": 2.55,
        "mcu_core_temp": 45,
        "heartbeat_signal": 1,
        "uart_packets_received": 100,
        "crc_error_count": 0,
        "uart_timeout": 0
    }
    print("--- Checking a Normal Sample ---")
    predict_fault_tflite(normal_sample, "Normal Sample")

    print("\n" + "-"*50 + "\n")

    # Example 2: A sample with overcurrent readings
    overcurrent_sample = {
        "bus_voltage": 4.85,
        "current_draw": 0.9,
        "power_consumption": 4.36,
        "mcu_core_temp": 60,
        "heartbeat_signal": 1,
        "uart_packets_received": 99,
        "crc_error_count": 1,
        "uart_timeout": 0
    }
    print("--- Checking an Overcurrent Fault Sample ---")
    predict_fault_tflite(overcurrent_sample, "Overcurrent Fault Sample")

    print("\n" + "-"*50 + "\n")

    # Example 3: A sample with a missed heartbeat
    no_heartbeat_sample = {
        "bus_voltage": 5.0,
        "current_draw": 0.5,
        "power_consumption": 2.5,
        "mcu_core_temp": 48,
        "heartbeat_signal": 0, # The fault condition
        "uart_packets_received": 99,
        "crc_error_count": 0,
        "uart_timeout": 0
    }
    print("--- Checking a No Heartbeat Fault Sample ---")
    predict_fault_tflite(no_heartbeat_sample, "No Heartbeat Fault Sample")
