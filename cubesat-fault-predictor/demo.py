#!/usr/bin/env python3
"""
Demo script for CubeSat Fault Predictor
Shows all the project functionality in one go
"""

import os
import sys
from pathlib import Path

# Add project paths
project_root = Path(__file__).parent
sys.path.append(str(project_root / 'src'))
sys.path.append(str(project_root / 'visualization'))

def main():
    print("🛰️  CubeSat Fault Predictor Demo")
    print("=" * 50)
    
    # Check if data exists
    data_file = project_root / 'data' / 'cubesat_data.csv'
    model_file = project_root / 'models' / 'fault_model.tflite'
    
    print(f"📊 Data file: {'✅ Found' if data_file.exists() else '❌ Missing'}")
    print(f"🤖 Model file: {'✅ Found' if model_file.exists() else '❌ Missing'}")
    print()
    
    if data_file.exists():
        # Import and run visualizations
        try:
            from visualize_data import CubeSatVisualizer
            print("🎨 Running visualizations...")
            
            viz = CubeSatVisualizer()
            print(f"📈 Loaded {len(viz.data)} samples")
            print(f"🔍 Fault distribution: {dict(viz.data['fault'].value_counts().sort_index())}")
            print()
            
            # Show quick stats
            print("📊 Quick Data Statistics:")
            print("-" * 30)
            numeric_cols = viz.data.select_dtypes(include=['float64', 'int64']).columns
            for col in numeric_cols:
                if col != 'fault':
                    mean_val = viz.data[col].mean()
                    std_val = viz.data[col].std()
                    print(f"{col:20}: {mean_val:8.2f} ± {std_val:6.2f}")
            
            print("\n🎯 To see interactive plots, run:")
            print("   python visualization/visualize_data.py")
            print("   python visualization/visualize_data.py --faults")
            print("   python visualization/visualize_data.py --correlation")
            
        except ImportError as e:
            print(f"❌ Could not import visualization: {e}")
    
    if model_file.exists():
        print(f"\n🤖 Model size: {model_file.stat().st_size / 1024:.1f} KB")
        print("🎯 To run predictions, use:")
        print("   python src/predict.py")
    
    print("\n✨ Project reorganized successfully!")
    print("📁 New structure:")
    print("   /data/          - Dataset")
    print("   /src/           - ML code")
    print("   /models/        - Trained models")
    print("   /visualization/ - Dynamic plots")

if __name__ == "__main__":
    main()