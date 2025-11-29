import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import os

class ResultAnalyzer:
    def __init__(self, results_file="large_scale_results.csv"):
        self.results_file = results_file
        if not os.path.exists(results_file):
            parent_path = os.path.join("..", results_file)
            if os.path.exists(parent_path):
                self.results_file = parent_path
            else:
                print(f"Warning: {results_file} not found in current or parent directory")
        self.df = None
        
    def load_data(self):
        try:
            self.df = pd.read_csv(self.results_file)
            print(f"Loaded data from {self.results_file}")
            print(f"Records: {len(self.df)}")
            print(f"Algorithms: {list(self.df['Algorithm'].unique())}")
            print(f"Graph sizes: {list(self.df['Vertices'].unique())}")
            print(f"Densities available: {sorted(self.df['Density'].unique())}")
            return self.df
        except FileNotFoundError:
            print(f"Results file {self.results_file} not found.")
            return None
        except Exception as e:
            print(f"Error loading data: {e}")
            return None
        
    def plot_scalability(self):
        if self.df is None:
            print("No data loaded")
            return
            
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        
        available_densities = sorted(self.df['Density'].unique())
        sparse_density = min(available_densities)  
        dense_density = max(available_densities)
        sparse_data = self.df[self.df['Density'] == sparse_density]
        if len(sparse_data) > 0:
            for algo in sparse_data['Algorithm'].unique():
                algo_data = sparse_data[sparse_data['Algorithm'] == algo]
                axes[0,0].plot(algo_data['Vertices'], algo_data['Time(ms)'], 
                              label=algo, marker='o', linewidth=2)
            axes[0,0].set_xlabel('Number of Vertices')
            axes[0,0].set_ylabel('Time (ms)')
            axes[0,0].set_title(f'Time Complexity vs Graph Size (Sparse: density={sparse_density})')
            axes[0,0].legend()
            axes[0,0].grid(True, alpha=0.3)
        else:
            axes[0,0].text(0.5, 0.5, 'No sparse graph data', 
                          ha='center', va='center', transform=axes[0,0].transAxes)
        
        dense_data = self.df[self.df['Density'] == dense_density]
        if len(dense_data) > 0:
            for algo in dense_data['Algorithm'].unique():
                algo_data = dense_data[dense_data['Algorithm'] == algo]
                axes[0,1].plot(algo_data['Vertices'], algo_data['Time(ms)'], 
                              label=algo, marker='s', linewidth=2)
            axes[0,1].set_xlabel('Number of Vertices')
            axes[0,1].set_ylabel('Time (ms)')
            axes[0,1].set_title(f'Time Complexity vs Graph Size (Dense: density={dense_density})')
            axes[0,1].legend()
            axes[0,1].grid(True, alpha=0.3)
        else:
            axes[0,1].text(0.5, 0.5, 'No dense graph data', 
                          ha='center', va='center', transform=axes[0,1].transAxes)
        
        memory_data = self.df[self.df['Memory(KB)'] > 0] 
        if len(memory_data) > 0:
            for algo in memory_data['Algorithm'].unique():
                algo_data = memory_data[memory_data['Algorithm'] == algo]
                avg_memory = algo_data.groupby('Vertices')['Memory(KB)'].mean()
                axes[1,0].plot(avg_memory.index, avg_memory.values, 
                              label=algo, marker='^', linewidth=2)
            axes[1,0].set_xlabel('Number of Vertices')
            axes[1,0].set_ylabel('Memory (KB)')
            axes[1,0].set_title('Memory Usage vs Graph Size')
            axes[1,0].legend()
            axes[1,0].grid(True, alpha=0.3)
        else:
            axes[1,0].text(0.5, 0.5, 'No memory data', 
                          ha='center', va='center', transform=axes[1,0].transAxes)
        
        available_sizes = sorted(self.df['Vertices'].unique())
        if available_sizes:
            medium_size = available_sizes[len(available_sizes) // 2]  
            medium_graphs = self.df[self.df['Vertices'] == medium_size]
            if len(medium_graphs) > 0:
                density_data = medium_graphs.groupby(['Algorithm', 'Density'])['Time(ms)'].mean().reset_index()
                for algo in density_data['Algorithm'].unique():
                    algo_data = density_data[density_data['Algorithm'] == algo]
                    axes[1,1].plot(algo_data['Density'], algo_data['Time(ms)'], 
                                  label=algo, marker='d', linewidth=2)
                axes[1,1].set_xlabel('Density (E/V)')
                axes[1,1].set_ylabel('Time (ms)')
                axes[1,1].set_title(f'Performance vs Graph Density (V={medium_size})')
                axes[1,1].legend()
                axes[1,1].grid(True, alpha=0.3)
                axes[1,1].set_xscale('log')
            else:
                axes[1,1].text(0.5, 0.5, f'No data for V={medium_size}', 
                              ha='center', va='center', transform=axes[1,1].transAxes)
        else:
            axes[1,1].text(0.5, 0.5, 'No graph size data', 
                          ha='center', va='center', transform=axes[1,1].transAxes)
        
        plt.tight_layout()
        plt.savefig('scalability_analysis_large.png', dpi=300, bbox_inches='tight')
        print("Saved scalability_analysis_large.png")
        plt.show()
    
    def generate_report(self):
        if self.df is None:
            print("No data to analyze")
            return
            
        print("=== MST Algorithms Performance Analysis ===\n")
        
        summary = self.df.groupby('Algorithm').agg({
            'Time(ms)': ['mean', 'std', 'min', 'max'],
            'Memory(KB)': ['mean', 'std'],
        }).round(2)
        
        print("Summary Statistics:")
        print(summary)
        print()
        
        print("=== Best Algorithm Recommendations ===")
        sparse = self.df[self.df['Density'] < 1.0]
        if len(sparse) > 0:
            sparse_times = sparse.groupby('Algorithm')['Time(ms)'].mean()
            if len(sparse_times) > 0:
                best_sparse = sparse_times.idxmin()
                print(f"Sparse graphs (density < 1.0): {best_sparse}")
        dense = self.df[self.df['Density'] >= 1.0]
        if len(dense) > 0:
            dense_times = dense.groupby('Algorithm')['Time(ms)'].mean()
            if len(dense_times) > 0:
                best_dense = dense_times.idxmin()
                print(f"Dense graphs (density >= 1.0): {best_dense}")
        
        memory_data = self.df[self.df['Memory(KB)'] > 0]
        if len(memory_data) > 0:
            memory_usage = memory_data.groupby('Algorithm')['Memory(KB)'].mean()
            if len(memory_usage) > 0:
                best_memory = memory_usage.idxmin()
                print(f"Most memory efficient: {best_memory}")
        
        large = self.df[self.df['Vertices'] >= 1000]
        if len(large) > 0:
            large_times = large.groupby('Algorithm')['Time(ms)'].mean()
            if len(large_times) > 0:
                best_large = large_times.idxmin()
                print(f"Large graphs (V >= 1000): {best_large}")
        
        print("\n=== Theoretical Complexities ===")
        complexities = {
            "Kruskal": "O(E log E)",
            "Prim": "O(E log V)", 
            "Prim_Parallel_2threads": "O(E log V) with parallel speedup",
            "Prim_Parallel_4threads": "O(E log V) with parallel speedup",
            "Karger-Klein-Tarjan": "O(E) expected"
        }

        existing_algorithms = set(self.df['Algorithm'].unique())
        for algo, complexity in complexities.items():
            if algo in existing_algorithms:
                print(f"{algo:<30}: {complexity}")

if __name__ == "__main__":
    analyzer = ResultAnalyzer()
    df = analyzer.load_data()
    
    if df is not None:
        analyzer.plot_scalability()
        analyzer.generate_report()