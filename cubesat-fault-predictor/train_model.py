'''
This script trains a TensorFlow neural network to predict CubeSat faults and
converts it to a TensorFlow Lite model for embedded deployment.
It also generates a plot of the training history.
'''
import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt
import os

# Configuration
DATA_FILE_PATH = "data/cubesat_data.csv"
MODELS_DIR = "models"
IMAGES_DIR = "images"
TFLITE_MODEL_PATH = os.path.join(MODELS_DIR, "fault_model.tflite")
HISTORY_PLOT_PATH = os.path.join(IMAGES_DIR, "training_history.png")

# Ensure output directories exist
os.makedirs(MODELS_DIR, exist_ok=True)
os.makedirs(IMAGES_DIR, exist_ok=True)

def plot_history(history):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12))

    # Plot training & validation accuracy values
    ax1.plot(history.history['accuracy'])
    ax1.plot(history.history['val_accuracy'])
    ax1.set_title('Model Accuracy')
    ax1.set_ylabel('Accuracy')
    ax1.set_xlabel('Epoch')
    ax1.legend(['Train', 'Validation'], loc='lower right')

    # Plot training & validation loss values
    ax2.plot(history.history['loss'])
    ax2.plot(history.history['val_loss'])
    ax2.set_title('Model Loss')
    ax2.set_ylabel('Loss')
    ax2.set_xlabel('Epoch')
    ax2.legend(['Train', 'Validation'], loc='upper right')

    plt.tight_layout()
    plt.savefig(HISTORY_PLOT_PATH)
    print(f"\nTraining history plot saved to {HISTORY_PLOT_PATH}")

def train_and_convert():
    # 1. Load and preprocess data
    print(f"Loading data from {DATA_FILE_PATH}...")
    try:
        df = pd.read_csv(DATA_FILE_PATH)
    except FileNotFoundError:
        print(f"Error: Data file not found. Please run generate_data.py first.")
        return

    features = [
        'bus_voltage', 'current_draw', 'power_consumption', 'mcu_core_temp', 'heartbeat_signal',
        'uart_packets_received', 'crc_error_count', 'uart_timeout'
    ]
    target = 'fault'

    X = df[features].values
    y = df[target].values

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42, stratify=y)

    # Scale the features
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.transform(X_test)

    # 2. Build the Keras model
    print("Building the neural network model...")
    model = tf.keras.Sequential([
        tf.keras.layers.Input(shape=(X_train.shape[1],)),
        tf.keras.layers.Dense(16, activation='relu'),
        tf.keras.layers.Dense(8, activation='relu'),
        tf.keras.layers.Dense(5, activation='softmax') # 5 classes (0-4)
    ])

    model.compile(optimizer='adam',
                  loss='sparse_categorical_crossentropy',
                  metrics=['accuracy'])

    model.summary()

    # 3. Train the model
    print("\nTraining the model...")
    history = model.fit(X_train, y_train, epochs=50, batch_size=32, validation_split=0.2, verbose=0)
    print("Training complete.")

    # 4. Plot training history
    plot_history(history)

    # 5. Evaluate the model
    print("\nEvaluating the Keras model...")
    loss, accuracy = model.evaluate(X_test, y_test, verbose=0)
    print(f"Keras model accuracy: {accuracy:.4f}")

    # 6. Convert to TensorFlow Lite model
    print(f"\nConverting to TensorFlow Lite model...")
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    tflite_model = converter.convert()

    # 7. Save the .tflite model
    with open(TFLITE_MODEL_PATH, 'wb') as f:
        f.write(tflite_model)

    model_size = os.path.getsize(TFLITE_MODEL_PATH)
    print(f"Successfully converted and saved model to {TFLITE_MODEL_PATH}")
    print(f"Final model size: {model_size / 1024:.2f} KB")

    if model_size > 50 * 1024:
        print("Warning: Model size exceeds the 50 KB target.")

if __name__ == "__main__":
    train_and_convert()