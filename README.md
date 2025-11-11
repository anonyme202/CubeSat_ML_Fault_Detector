# CubeSat ML Fault Detector

A comprehensive machine learning-based fault detection and prediction sub-system for CubeSat satellites. This project combines Python-based ML model training with embedded C firmware for real-time fault diagnostics on STM32H735 microcontroller hardware.

## 🎯 Overview

This project implements an end-to-end fault prediction system for CubeSats, capable of detecting and classifying five different operational states:

- **Normal Operation**: System functioning within expected parameters
- **OBC - No Heartbeat**: On-Board Computer heartbeat signal missing
- **OBC - Overcurrent**: Excessive current draw detected
- **TTC - UART Timeout**: Transceiver/Telecommand communication timeout
- **TTC - Data Corruption**: CRC/checksum mismatch in received data

The system uses a neural network trained on simulated CubeSat telemetry data and converts it to TensorFlow Lite format for deployment on resource-constrained embedded systems.

## 📁 Project Structure

```
CubeSat_ML_Fault_Detector/
├── cubesat-fault-predictor/          # Python ML pipeline
│   ├── generate_data.py              # Simulate realistic CubeSat telemetry
│   ├── train_model.py                # Train neural network & convert to TFLite
│   ├── predict.py                    # Run inference with TFLite model
│   ├── demo.py                       # Interactive demonstration
│   ├── visualize_data.py             # Data exploration and visualization
│   ├── requirements.txt              # Python dependencies
│   ├── data/                         # Generated training datasets
│   ├── models/                       # Trained models (TFLite format)
│   └── images/                       # Training plots & visualizations
│
├── firmware/                         # STM32H735 embedded deployment
│   ├── CubeSat_ML_Fault_Detector.ioc # STM32CubeIDE project file
│   ├── core/                         # Microcontroller code
│   ├── Drivers/                      # HAL drivers & CMSIS
│   ├── Middlewares/                  # X-CUBE-AI (ST's ML inference)
│   └── Startup/                      # Bootloader
│
├── hardware/                         # PCB design & fabrication
│   └── pcb/
│       ├── design files/             # Altium Designer schematics & layouts
│       └── Fabrication files/        # Gerber files for manufacturing
│
└── README.md                         # This file
```

## 🚀 Quick Start

### Prerequisites

- **Python 3.8+**
- **pip** (Python package manager)
- **STM32CubeIDE** (for firmware deployment, optional)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/anonyme202/CubeSat_ML_Fault_Detector.git
   cd CubeSat_ML_Fault_Detector/cubesat-fault-predictor
   ```

2. **Install Python dependencies**
   ```bash
   pip install -r requirements.txt
   ```

3. **Generate training data**
   ```bash
   python generate_data.py
   ```
   This creates `data/cubesat_data.csv` with 5,000 simulated telemetry samples.

4. **Train the model**
   ```bash
   python train_model.py
   ```
   - Trains a neural network on the generated data
   - Generates training history plots
   - Converts to TensorFlow Lite format: `models/fault_model.tflite`

5. **Run predictions**
   ```bash
   python predict.py
   ```
   Tests the trained model on sample telemetry data and generates confidence plots.

6. **Try the interactive demo** (optional)
   ```bash
   python demo.py
   ```

## 📊 ML Pipeline

### Data Generation (`generate_data.py`)

Generates synthetic CubeSat telemetry data with realistic normal operation patterns and fault injection:

**Features:**
- `bus_voltage`: Power bus voltage (V)
- `current_draw`: System current consumption (A)
- `mcu_core_temp`: Microcontroller temperature (°C)
- `heartbeat_signal`: OBC heartbeat status (0/1)
- `uart_packets_received`: Valid UART packets per sample window
- `crc_error_count`: Detected CRC errors
- `uart_timeout`: UART communication timeout flag

**Fault Distribution:**
- 80% Normal operation
- 5% OBC No heartbeat
- 5% OBC Overcurrent
- 5% TTC UART timeout
- 5% TTC Data corruption

### Model Training (`train_model.py`)

- **Input**: Generated telemetry features
- **Architecture**: Multi-layer dense neural network
- **Output**: 5-class softmax classification
- **Framework**: TensorFlow 2.x
- **Deployment Format**: TensorFlow Lite (optimized for embedded systems)

**Output Files:**
- `models/fault_model.tflite`: Quantized model for microcontroller
- `images/training_history.png`: Accuracy and loss curves

### Inference (`predict.py`)

Demonstrates real-time fault prediction using the TFLite model with:
- Sample telemetry input scenarios
- Confidence scores for each fault class
- Visualization of prediction confidence

## 🔧 Firmware Deployment

### Building with STM32CubeIDE

1. Open `firmware/CubeSat_ML_Fault_Detector.ioc` in STM32CubeIDE
2. Generate code and compile
3. Flash to STM32H735 device

### Key Components

- **X-CUBE-AI Integration**: ST's ML inference engine for deploying TFLite models
- **FreeRTOS**: Real-time operating system for task scheduling
- **UART/I2C**: Communication peripherals for telemetry collection
- **ADC**: Analog-to-digital conversion for sensor inputs

## 📋 Dependencies

### Python
```
pandas>=1.3.0          # Data manipulation
scikit-learn>=1.0.0    # Preprocessing & ML utilities
tensorflow>=2.8.0      # Deep learning framework
matplotlib>=3.5.0      # Visualization
seaborn>=0.11.0        # Statistical plots
numpy>=1.21.0          # Numerical computing
```

### Firmware
- STM32H735 HAL drivers
- ST X-CUBE-AI middleware
- FreeRTOS RTOS
- CMSIS libraries

## 💡 Usage Examples

### Training a Custom Model

```python
python generate_data.py  # Generate 5000 samples
python train_model.py    # Train and convert to TFLite
```

### Batch Prediction

The `predict.py` script can be extended for batch inference on telemetry streams.

### Real-time Monitoring

Deploy the TFLite model on the STM32H735 to perform real-time fault detection with <10ms latency.

## 📈 Performance Metrics

After training, the model achieves:
- **Accuracy**: Displayed in `images/training_history.png`
- **Inference Speed**: ~5-10ms on STM32H735 (ARM Cortex-M7)
- **Model Size**: ~50KB (TFLite binary)
- **Memory Usage**: ~100KB RAM footprint

## 🛠️ Development Workflow

1. **Data Generation**: Iterate on fault scenarios
2. **Training**: Tune hyperparameters and model architecture
3. **Validation**: Test on simulated telemetry
4. **Deployment**: Convert to TFLite and embed in firmware
5. **Testing**: Hardware validation with actual sensor data

## 🔗 Related Technologies

- **STM32H735**: High-performance ARM Cortex-M7 microcontroller
- **TensorFlow Lite**: Lightweight ML framework for embedded systems
- **X-CUBE-AI**: ST's neural network development package
- **FreeRTOS**: Open-source RTOS for embedded systems

## 📝 License

See LICENSE file in the repository root.

## 👤 Author

anonyme202

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## 📧 Support

For issues, questions, or suggestions, please open an GitHub issue in the repository.