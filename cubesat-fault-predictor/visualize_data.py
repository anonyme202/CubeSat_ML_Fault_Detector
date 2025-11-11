#!/usr/bin/env python3
"""
CubeSat Fault Predictor - Data Visualization
Minimal dynamic visualization for CubeSat telemetry data and model results
"""

import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
from pathlib import Path
import sys

# Add src to path for imports
sys.path.append(str(Path(__file__).parent.parent / 'src'))

class CubeSatVisualizer:
    def __init__(self, data_path=None):
        """Initialize visualizer with data path"""
        if data_path is None:
            self.data_path = Path(__file__).parent.parent / 'data' / 'cubesat_data.csv'
        else:
            self.data_path = Path(data_path)
        
        self.data = None
        self.load_data()
    
    def load_data(self):
        """Load CubeSat telemetry data"""
        try:
            self.data = pd.read_csv(self.data_path)
            print(f"Loaded {len(self.data)} samples from {self.data_path}")
        except FileNotFoundError:
            print(f"Data file not found: {self.data_path}")
            return False
        return True
    
    def plot_sensor_distributions(self, save=False):
        """Plot distribution of all sensor readings"""
        if self.data is None:
            return
        
        # Select numeric columns (exclude fault column for now)
        numeric_cols = self.data.select_dtypes(include=[np.number]).columns
        sensor_cols = [col for col in numeric_cols if col != 'fault']
        
        fig, axes = plt.subplots(2, 4, figsize=(16, 8))
        axes = axes.flatten()
        
        for i, col in enumerate(sensor_cols):
            if i < len(axes):
                axes[i].hist(self.data[col], bins=30, alpha=0.7, edgecolor='black')
                axes[i].set_title(f'{col.replace("_", " ").title()}')
                axes[i].set_xlabel('Value')
                axes[i].set_ylabel('Frequency')
        
        # Hide unused subplots
        for i in range(len(sensor_cols), len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        plt.suptitle('CubeSat Sensor Reading Distributions', y=1.02, fontsize=16)
        
        if save:
            plt.savefig(Path(__file__).parent / 'sensor_distributions.png', dpi=300, bbox_inches='tight')
        
        plt.show()
    
    def plot_fault_analysis(self, save=False):
        """Analyze fault patterns"""
        if self.data is None:
            return
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        # Fault distribution
        fault_counts = self.data['fault'].value_counts().sort_index()
        axes[0, 0].bar(fault_counts.index, fault_counts.values, color='skyblue', edgecolor='black')
        axes[0, 0].set_title('Fault Distribution')
        axes[0, 0].set_xlabel('Fault Type')
        axes[0, 0].set_ylabel('Count')
        
        # Fault labels for clarity
        fault_labels = {0: 'Normal', 1: 'Minor', 2: 'Power', 3: 'Comm', 4: 'CRC'}
        axes[0, 0].set_xticks(list(fault_labels.keys()))
        axes[0, 0].set_xticklabels([fault_labels.get(i, str(i)) for i in fault_labels.keys()])
        
        # Power consumption by fault type
        axes[0, 1].boxplot([self.data[self.data['fault'] == i]['power_consumption'] 
                           for i in sorted(self.data['fault'].unique())], 
                          labels=[fault_labels.get(i, str(i)) for i in sorted(self.data['fault'].unique())])
        axes[0, 1].set_title('Power Consumption by Fault Type')
        axes[0, 1].set_ylabel('Power Consumption')
        
        # Temperature vs Current Draw colored by fault
        scatter = axes[1, 0].scatter(self.data['current_draw'], self.data['mcu_core_temp'], 
                                   c=self.data['fault'], cmap='viridis', alpha=0.6)
        axes[1, 0].set_xlabel('Current Draw')
        axes[1, 0].set_ylabel('MCU Core Temperature')
        axes[1, 0].set_title('Temperature vs Current Draw')
        plt.colorbar(scatter, ax=axes[1, 0], label='Fault Type')
        
        # CRC Errors vs UART Timeouts
        axes[1, 1].scatter(self.data['crc_error_count'], self.data['uart_timeout'], 
                          c=self.data['fault'], cmap='plasma', alpha=0.6)
        axes[1, 1].set_xlabel('CRC Error Count')
        axes[1, 1].set_ylabel('UART Timeout')
        axes[1, 1].set_title('Communication Errors')
        
        plt.tight_layout()
        
        if save:
            plt.savefig(Path(__file__).parent / 'fault_analysis.png', dpi=300, bbox_inches='tight')
        
        plt.show()
    
    def plot_correlation_matrix(self, save=False):
        """Plot correlation matrix of all features"""
        if self.data is None:
            return
        
        # Calculate correlation matrix
        corr_matrix = self.data.corr()
        
        plt.figure(figsize=(10, 8))
        sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, 
                   square=True, fmt='.2f', cbar_kws={'shrink': 0.8})
        plt.title('Feature Correlation Matrix')
        plt.tight_layout()
        
        if save:
            plt.savefig(Path(__file__).parent / 'correlation_matrix.png', dpi=300, bbox_inches='tight')
        
        plt.show()
    
    def plot_time_series_simulation(self, sample_size=100, save=False):
        """Simulate and plot time series data"""
        if self.data is None:
            return
        
        # Take a random sample and simulate time progression
        sample_data = self.data.sample(n=min(sample_size, len(self.data))).reset_index(drop=True)
        sample_data['timestamp'] = range(len(sample_data))
        
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        
        # Power system metrics
        axes[0].plot(sample_data['timestamp'], sample_data['bus_voltage'], label='Bus Voltage', color='blue')
        axes[0].plot(sample_data['timestamp'], sample_data['current_draw'], label='Current Draw', color='red')
        axes[0].set_title('Power System Telemetry Over Time')
        axes[0].set_ylabel('Value')
        axes[0].legend()
        axes[0].grid(True, alpha=0.3)
        
        # Communication metrics
        axes[1].plot(sample_data['timestamp'], sample_data['uart_packets_received'], label='UART Packets', color='green')
        axes[1].plot(sample_data['timestamp'], sample_data['crc_error_count'], label='CRC Errors', color='orange')
        axes[1].set_title('Communication System Telemetry')
        axes[1].set_ylabel('Count')
        axes[1].legend()
        axes[1].grid(True, alpha=0.3)
        
        # Fault status and temperature
        axes[2].plot(sample_data['timestamp'], sample_data['mcu_core_temp'], label='MCU Temperature', color='purple')
        fault_scatter = axes[2].scatter(sample_data['timestamp'], sample_data['fault'] * 10, 
                                      c=sample_data['fault'], cmap='Reds', 
                                      label='Fault Status (x10)', alpha=0.7, s=30)
        axes[2].set_title('System Health Over Time')
        axes[2].set_xlabel('Time Step')
        axes[2].set_ylabel('Temperature / Fault Level')
        axes[2].legend()
        axes[2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save:
            plt.savefig(Path(__file__).parent / 'time_series_simulation.png', dpi=300, bbox_inches='tight')
        
        plt.show()
    
    def generate_all_plots(self, save=False):
        """Generate all visualization plots"""
        print("Generating CubeSat telemetry visualizations...")
        
        self.plot_sensor_distributions(save=save)
        self.plot_fault_analysis(save=save)
        self.plot_correlation_matrix(save=save)
        self.plot_time_series_simulation(save=save)
        
        print("All visualizations complete!")

def main():
    """Main function for running visualizations"""
    visualizer = CubeSatVisualizer()
    
    if len(sys.argv) > 1:
        if sys.argv[1] == '--save':
            visualizer.generate_all_plots(save=True)
        elif sys.argv[1] == '--distributions':
            visualizer.plot_sensor_distributions()
        elif sys.argv[1] == '--faults':
            visualizer.plot_fault_analysis()
        elif sys.argv[1] == '--correlation':
            visualizer.plot_correlation_matrix()
        elif sys.argv[1] == '--timeseries':
            visualizer.plot_time_series_simulation()
        else:
            print("Usage: python visualize_data.py [--save|--distributions|--faults|--correlation|--timeseries]")
    else:
        # Interactive mode - show all plots
        visualizer.generate_all_plots()

if __name__ == "__main__":
    main()