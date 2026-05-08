#!/usr/bin/env python3
"""
Benchmark Plotting Script for NanoDB
Reads benchmark CSV and generates plots for research report.
"""

import matplotlib.pyplot as plt
import csv
import os

def read_benchmark_csv(filename):
    """Read benchmark results from CSV file."""
    data = []
    with open(filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            data.append({
                'operation': row['operation'],
                'size': int(row['size']),
                'time_ms': float(row['time_ms'])
            })
    return data

def plot_insertion_benchmark(data):
    """Plot insertion time vs record count."""
    insertion_data = [d for d in data if d['operation'] == 'insertion']
    
    sizes = [d['size'] for d in insertion_data]
    times = [d['time_ms'] for d in insertion_data]
    
    plt.figure(figsize=(10, 6))
    plt.plot(sizes, times, marker='o', linewidth=2, markersize=8)
    plt.xlabel('Record Count', fontsize=12)
    plt.ylabel('Time (ms)', fontsize=12)
    plt.title('Insertion Time vs Record Count', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    plt.yscale('log')
    
    os.makedirs('graphs', exist_ok=True)
    plt.savefig('graphs/insertion_benchmark.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("[Plot] Saved graphs/insertion_benchmark.png")

def plot_scan_comparison(data):
    """Plot indexed vs sequential scan time vs record count."""
    indexed_data = [d for d in data if d['operation'] == 'indexed_scan']
    sequential_data = [d for d in data if d['operation'] == 'sequential_scan']
    
    indexed_sizes = [d['size'] for d in indexed_data]
    indexed_times = [d['time_ms'] for d in indexed_data]
    
    sequential_sizes = [d['size'] for d in sequential_data]
    sequential_times = [d['time_ms'] for d in sequential_data]
    
    plt.figure(figsize=(10, 6))
    plt.plot(indexed_sizes, indexed_times, marker='o', linewidth=2, markersize=8, label='Indexed (AVL)')
    plt.plot(sequential_sizes, sequential_times, marker='s', linewidth=2, markersize=8, label='Sequential')
    plt.xlabel('Record Count', fontsize=12)
    plt.ylabel('Time (ms)', fontsize=12)
    plt.title('Indexed vs Sequential Scan Performance', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.xscale('log')
    plt.yscale('log')
    
    os.makedirs('graphs', exist_ok=True)
    plt.savefig('graphs/scan_comparison.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("[Plot] Saved graphs/scan_comparison.png")

def plot_lru_profile(data):
    """Plot LRU page fault rate vs memory pool size."""
    lru_data = [d for d in data if d['operation'] == 'lru_scan']
    
    if not lru_data:
        print("[Plot] No LRU data found, skipping LRU plot")
        return
    
    # For LRU, we'll plot the single data point as a bar
    sizes = [d['size'] for d in lru_data]
    times = [d['time_ms'] for d in lru_data]
    
    plt.figure(figsize=(10, 6))
    plt.bar(['LRU Scan'], times, color='steelblue', alpha=0.7)
    plt.ylabel('Time (ms)', fontsize=12)
    plt.title(f'LRU Stress Test: {sizes[0]} Records Scanned', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, axis='y')
    
    os.makedirs('graphs', exist_ok=True)
    plt.savefig('graphs/lru_profile.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("[Plot] Saved graphs/lru_profile.png")

def main():
    """Main plotting function."""
    csv_filename = 'benchmark_results.csv'
    
    if not os.path.exists(csv_filename):
        print(f"[ERROR] {csv_filename} not found. Run benchmarks first.")
        return
    
    print("[Plot] Reading benchmark data from", csv_filename)
    data = read_benchmark_csv(csv_filename)
    
    if not data:
        print("[ERROR] No data found in", csv_filename)
        return
    
    print(f"[Plot] Loaded {len(data)} benchmark results")
    
    plot_insertion_benchmark(data)
    plot_scan_comparison(data)
    plot_lru_profile(data)
    
    print("[Plot] All plots generated successfully in graphs/ directory")

if __name__ == "__main__":
    main()
