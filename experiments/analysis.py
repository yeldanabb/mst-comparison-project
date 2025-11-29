import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import os

class AdvancedAnalyzer:
    def __init__(self, results_file="comprehensive_results.csv"):
        self.results_file = results_file
        if not os.path.exists(results_file):
            parent_path = os.path.join("..", results_file)
            if os.path.exists(parent_path):
                self.results_file = parent_path
        self.df = None
        
    def load_data(self):
        try:
            self.df = pd.read_csv(self.results_file)
            print(f"Loaded {len(self.df)} records from {self.results_file}")
            print(f"Algorithms: {list(self.df['Algorithm'].unique())}")
            print(f"Graph sizes: {sorted(self.df['Vertices'].unique())}")
            print(f"Densities: {sorted(self.df['Density'].unique())}")
            return self.df
        except Exception as e:
            print(f"Error loading data: {e}")
            return None
    
    def plot_comprehensive_analysis(self):
        if self.df is None:
            return
            
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        
        self._plot_time_complexity(axes[0,0])
        
        self._plot_density_analysis(axes[0,1])
        
        self._plot_memory_analysis(axes[0,2])
        
        self._plot_performance_heatmap(axes[1,0])
        
        self._plot_speedup_analysis(axes[1,1])
        
        self._plot_complexity_verification(axes[1,2])
        
        plt.tight_layout()
        plt.savefig('comprehensive_analysis.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def _plot_time_complexity(self, ax):
        sparse_data = self.df[self.df['Density'] == 0.1]
        for algo in sparse_data['Algorithm'].unique():
            algo_data = sparse_data[sparse_data['Algorithm'] == algo]
            vertices = algo_data['Vertices']
            times = algo_data['Time(ms)']
            
            ax.loglog(vertices, times, 'o-', label=algo, alpha=0.7)
            
            if len(vertices) > 2:
                log_v = np.log(vertices)
                log_t = np.log(times)
                slope = np.polyfit(log_v, log_t, 1)[0]
                ax.text(vertices.iloc[-1], times.iloc[-1], 
                       f' n^{slope:.2f}', fontsize=8)
        
        ax.set_xlabel('Number of Vertices (log scale)')
        ax.set_ylabel('Time (ms) (log scale)')
        ax.set_title('Time Complexity Verification\n(Log-Log Plot, Sparse Graphs)')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_density_analysis(self, ax):
        medium_large = self.df[self.df['Vertices'] >= 1000]
        for algo in medium_large['Algorithm'].unique():
            algo_data = medium_large[medium_large['Algorithm'] == algo]
            density_avg = algo_data.groupby('Density')['Time(ms)'].mean()
            ax.plot(density_avg.index, density_avg.values, 's-', label=algo, alpha=0.7)
        
        ax.set_xlabel('Graph Density (E/V)')
        ax.set_ylabel('Average Time (ms)')
        ax.set_title('Performance vs Graph Density\n(V ≥ 1000)')
        ax.legend()
        ax.grid(True, alpha=0.3)
        ax.set_xscale('log')
    
    def _plot_memory_analysis(self, ax):
        valid_memory = self.df[self.df['Memory(KB)'] > 0]
        for algo in valid_memory['Algorithm'].unique():
            algo_data = valid_memory[valid_memory['Algorithm'] == algo]
            mem_avg = algo_data.groupby('Vertices')['Memory(KB)'].mean()
            ax.plot(mem_avg.index, mem_avg.values / 1024, '^-', label=algo, alpha=0.7) 
        
        ax.set_xlabel('Number of Vertices')
        ax.set_ylabel('Memory Usage (MB)')
        ax.set_title('Memory Usage Analysis')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_performance_heatmap(self, ax):
        pivot_data = self.df.pivot_table(
            values='Time(ms)', 
            index='Vertices', 
            columns='Algorithm', 
            aggfunc='mean'
        )
        
        normalized = pivot_data.div(pivot_data.min(axis=1), axis=0)
        im = ax.imshow(normalized.values, cmap='RdYlGn_r', aspect='auto', vmin=1, vmax=3)
        ax.set_xticks(range(len(normalized.columns)))
        ax.set_xticklabels(normalized.columns, rotation=45, ha='right')
        ax.set_yticks(range(len(normalized.index)))
        ax.set_yticklabels(normalized.index)
        
        ax.set_xlabel('Algorithm')
        ax.set_ylabel('Vertices')
        ax.set_title('Performance Heatmap\n(Normalized to fastest)')
        
        plt.colorbar(im, ax=ax, label='Relative Time (lower is better)')
    
    def _plot_speedup_analysis(self, ax):
        parallel_algorithms = [algo for algo in self.df['Algorithm'].unique() if 'Parallel' in algo]
        if not parallel_algorithms:
            ax.text(0.5, 0.5, 'No parallel algorithms found', 
                   ha='center', va='center', transform=ax.transAxes)
            ax.set_title('Parallel Speedup Analysis')
            return
        
        baseline = 'Prim_BinaryHeap'
        large_graphs = self.df[self.df['Vertices'] >= 5000]
        
        baseline_times = large_graphs[large_graphs['Algorithm'] == baseline]
        
        for parallel_algo in parallel_algorithms:
            parallel_times = large_graphs[large_graphs['Algorithm'] == parallel_algo]
            speedups = []
            vertices_list = []
            
            for vertices in sorted(large_graphs['Vertices'].unique()):
                base_time = baseline_times[baseline_times['Vertices'] == vertices]['Time(ms)'].mean()
                parallel_time = parallel_times[parallel_times['Vertices'] == vertices]['Time(ms)'].mean()
                
                if base_time > 0 and parallel_time > 0:
                    speedup = base_time / parallel_time
                    speedups.append(speedup)
                    vertices_list.append(vertices)
            
            if speedups:
                ax.plot(vertices_list, speedups, 'o-', label=parallel_algo)
        
        ax.axhline(y=1, color='red', linestyle='--', alpha=0.5, label='Baseline')
        ax.set_xlabel('Number of Vertices')
        ax.set_ylabel('Speedup (vs Prim_BinaryHeap)')
        ax.set_title('Parallel Algorithm Speedup')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_complexity_verification(self, ax):
        sparse_data = self.df[self.df['Density'] == 0.1]
        vertices = sorted(sparse_data['Vertices'].unique())
        x_vals = np.array(vertices)
        ax.loglog(x_vals, x_vals * np.log(x_vals), 'k--', label='O(n log n)', alpha=0.5)
        ax.loglog(x_vals, x_vals * np.log(x_vals) * np.log(x_vals), 'k:', label='O(n log² n)', alpha=0.5)
        ax.loglog(x_vals, x_vals, 'k-.', label='O(n)', alpha=0.5)
        
        for algo in sparse_data['Algorithm'].unique():
            algo_data = sparse_data[sparse_data['Algorithm'] == algo]
            times = algo_data.groupby('Vertices')['Time(ms)'].mean()
            ax.loglog(times.index, times.values, 'o-', label=algo, alpha=0.7)
        
        ax.set_xlabel('Vertices')
        ax.set_ylabel('Time (ms)')
        ax.set_title('Theoretical vs Actual Complexity')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def generate_detailed_report(self):
        if self.df is None:
            return
            
        print("-"*70)
        print("MSt Algorithms performance report")
        print("-"*70)
        
        stats = self.df.groupby('Algorithm').agg({
            'Time(ms)': ['count', 'mean', 'std', 'min', 'max'],
            'Memory(KB)': ['mean', 'std']
        }).round(2)
        
        print("\nPERFORMANCE STATISTICS:")
        print(stats)
        
        print("\nALGORITHM RECOMMENDATIONS:")
        
        categories = [
            ('Sparse Graphs (density < 0.1)', self.df[self.df['Density'] < 0.1]),
            ('Medium Graphs (0.1 ≤ density < 1.0)', self.df[(self.df['Density'] >= 0.1) & (self.df['Density'] < 1.0)]),
            ('Dense Graphs (density ≥ 1.0)', self.df[self.df['Density'] >= 1.0]),
            ('Large Graphs (V ≥ 10000)', self.df[self.df['Vertices'] >= 10000]),
            ('Memory Efficient', self.df[self.df['Memory(KB)'] > 0])
        ]
        
        for category_name, category_data in categories:
            if len(category_data) > 0:
                if 'Memory' in category_name:
                    best = category_data.groupby('Algorithm')['Memory(KB)'].mean().idxmin()
                else:
                    best = category_data.groupby('Algorithm')['Time(ms)'].mean().idxmin()
                print(f"  {category_name}: {best}")
        
        print("\nCOMPLEXITY VERIFICATION:")
        sparse_large = self.df[(self.df['Density'] == 0.1) & (self.df['Vertices'] >= 1000)]
        for algo in sparse_large['Algorithm'].unique():
            algo_data = sparse_large[sparse_large['Algorithm'] == algo]
            if len(algo_data) > 2:
                vertices = algo_data['Vertices']
                times = algo_data['Time(ms)']
                log_v = np.log(vertices)
                log_t = np.log(times)
                slope = np.polyfit(log_v, log_t, 1)[0]
                print(f"  {algo:<25}: Empirical exponent = {slope:.3f}")

if __name__ == "__main__":
    analyzer = AdvancedAnalyzer()
    df = analyzer.load_data()
    
    if df is not None:
        analyzer.plot_comprehensive_analysis()
        analyzer.generate_detailed_report()