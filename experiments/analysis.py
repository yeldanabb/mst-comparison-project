import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

class SimpleAnalyzer:
    def __init__(self, results_file="comprehensive_results.csv"):
        self.results_file = results_file
        self.df = None
        
    def load_data(self):
        try:
            self.df = pd.read_csv(self.results_file)
            return True
        except:
            try:
                self.df = pd.read_csv("large_scale_results.csv")
                return True
            except:
                print("No results file found")
                return False
    
    def analyze_and_plot(self):
        if self.df is None:
            return
            
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        
        self.plot_performance_bars(axes[0,0])
        
        self.plot_kk_analysis(axes[0,1])
        
        self.plot_scalability(axes[0,2])
        
        self.plot_density_effect(axes[1,0])
        
        self.plot_parallel_efficiency(axes[1,1])
        
        self.plot_best_cases(axes[1,2])
        
        plt.tight_layout()
        plt.savefig('quick_analysis.png', dpi=150, bbox_inches='tight')
        plt.show()
        
    
    def plot_performance_bars(self, ax):
        avg_times = self.df.groupby('Algorithm')['Time(ms)'].mean().sort_values()
        colors = ['red' if 'KKT' in algo else 'blue' if 'Parallel' in algo else 'green' for algo in avg_times.index]
        
        bars = ax.bar(range(len(avg_times)), avg_times.values, color=colors, alpha=0.7)
        ax.set_xticks(range(len(avg_times)))
        ax.set_xticklabels(avg_times.index, rotation=45, ha='right')
        ax.set_ylabel('Average Time (ms)')
        ax.set_title('Algorithm Performance')
        ax.grid(True, alpha=0.3)
        
        for i, v in enumerate(avg_times.values):
            ax.text(i, v, f'{v:.0f}ms', ha='center', va='bottom', rotation=90)
    
    def plot_kk_analysis(self, ax):
        kkt_data = self.df[self.df['Algorithm'] == 'KKT']
        prim_data = self.df[self.df['Algorithm'] == 'Prim_BinaryHeap']
        
        ratios = []
        sizes = []
        
        for size in sorted(self.df['Vertices'].unique()):
            kkt_times = kkt_data[kkt_data['Vertices'] == size]['Time(ms)']
            prim_times = prim_data[prim_data['Vertices'] == size]['Time(ms)']
            
            if len(kkt_times) > 0 and len(prim_times) > 0:
                ratio = kkt_times.mean() / prim_times.mean()
                ratios.append(ratio)
                sizes.append(size)
        
        ax.bar(range(len(sizes)), ratios, color='red', alpha=0.7)
        ax.set_xticks(range(len(sizes)))
        ax.set_xticklabels([f'V{s}' for s in sizes])
        ax.set_ylabel('KKT Time / Prim Time')
        ax.set_title('KKT Performance Gap')
        ax.axhline(y=1, color='black', linestyle='--')
        ax.grid(True, alpha=0.3)
        
        for i, ratio in enumerate(ratios):
            ax.text(i, ratio, f'{ratio:.1f}x', ha='center', va='bottom')
    
    def plot_scalability(self, ax):
        sparse_data = self.df[self.df['Density'] < 0.1]
        
        for algo in ['Prim_BinaryHeap', 'KKT', 'Boruvka_Parallel_4threads']:
            if algo in sparse_data['Algorithm'].values:
                algo_data = sparse_data[sparse_data['Algorithm'] == algo]
                performance = algo_data.groupby('Vertices')['Time(ms)'].mean()
                ax.loglog(performance.index, performance.values, 'o-', label=algo, markersize=4)
        
        ax.set_xlabel('Vertices')
        ax.set_ylabel('Time (ms)')
        ax.set_title('Scalability (Sparse Graphs)')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def plot_density_effect(self, ax):
        large_graphs = self.df[self.df['Vertices'] >= 1000]
        
        for algo in ['Prim_BinaryHeap', 'KKT']:
            if algo in large_graphs['Algorithm'].values:
                algo_data = large_graphs[large_graphs['Algorithm'] == algo]
                density_perf = algo_data.groupby('Density')['Time(ms)'].mean()
                ax.plot(density_perf.index, density_perf.values, 's-', label=algo, markersize=4)
        
        ax.set_xlabel('Density')
        ax.set_ylabel('Time (ms)')
        ax.set_title('Density Effect (V≥1000)')
        ax.legend()
        ax.set_xscale('log')
        ax.set_yscale('log')
        ax.grid(True, alpha=0.3)
    
    def plot_parallel_efficiency(self, ax):
        parallel_algos = [algo for algo in self.df['Algorithm'].unique() if 'Parallel' in algo]
        
        if not parallel_algos:
            ax.text(0.5, 0.5, 'No parallel algorithms\ntested', 
                   ha='center', va='center', transform=ax.transAxes, fontsize=12)
            ax.set_title('Parallel Efficiency')
            return
        
        speedups = []
        algo_names = []
        
        for parallel_algo in parallel_algos:
            parallel_avg = self.df[self.df['Algorithm'] == parallel_algo]['Time(ms)'].mean()
            prim_avg = self.df[self.df['Algorithm'] == 'Prim_BinaryHeap']['Time(ms)'].mean()
            
            if parallel_avg > 0:
                speedup = prim_avg / parallel_avg
                speedups.append(speedup)
                algo_names.append(parallel_algo)
        
        if speedups:
            bars = ax.bar(range(len(speedups)), speedups, color='orange', alpha=0.7)
            ax.set_xticks(range(len(speedups)))
            ax.set_xticklabels(algo_names, rotation=45, ha='right')
            ax.set_ylabel('Speedup vs Prim')
            ax.set_title('Parallel Speedup')
            ax.axhline(y=1, color='red', linestyle='--', label='Baseline')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            for i, speedup in enumerate(speedups):
                ax.text(i, speedup, f'{speedup:.2f}x', ha='center', va='bottom')
    
    def plot_best_cases(self, ax):
        best_cases = []
        for algo in self.df['Algorithm'].unique():
            algo_data = self.df[self.df['Algorithm'] == algo]
            best_time = algo_data['Time(ms)'].min()
            best_case = algo_data[algo_data['Time(ms)'] == best_time].iloc[0]
            best_cases.append({
                'Algorithm': algo,
                'Time': best_time,
                'Vertices': best_case['Vertices'],
                'Density': best_case['Density']
            })
        
        best_df = pd.DataFrame(best_cases).sort_values('Time')
        
        labels = [f"{row['Algorithm']}\nV{row['Vertices']}" for _, row in best_df.iterrows()]
        bars = ax.bar(range(len(best_df)), best_df['Time'], alpha=0.7)
        ax.set_xticks(range(len(best_df)))
        ax.set_xticklabels(labels, rotation=45, ha='right')
        ax.set_ylabel('Best Time (ms)')
        ax.set_title('Best Case Performance')
        ax.grid(True, alpha=0.3)
    

if __name__ == "__main__":
    analyzer = SimpleAnalyzer()
    if analyzer.load_data():
        analyzer.analyze_and_plot()