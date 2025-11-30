import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import os

class SimpleResultAnalyzer:
    def __init__(self, results_file="large_scale_results.csv"):
        self.results_file = results_file
        self.df = None
        plt.style.use('default')
        plt.rcParams['figure.figsize'] = [15, 10]
        
    def load_data(self):
        try:
            self.df = pd.read_csv(self.results_file)
            print(f"Loaded {len(self.df)} records")
            print(f"Algorithms: {self.df['Algorithm'].unique()}")
            return True
        except Exception as e:
            print(f"Error: {e}")
            return False
    
    def create_simple_report(self):
        if self.df is None:
            return
            
        fig, axes = plt.subplots(2, 2)
        
        self._plot_overall_performance(axes[0,0])
        
        self._plot_scalability(axes[0,1])
         
        self._plot_density_performance(axes[1,0])
        
        self._plot_algorithm_ranking(axes[1,1])
        
        plt.tight_layout()
        plt.savefig('simple_analysis.png', dpi=150, bbox_inches='tight')
        plt.show()
        
    
    def _plot_overall_performance(self, ax):
        avg_times = self.df.groupby('Algorithm')['Time(ms)'].mean().sort_values()
        colors = ['skyblue' if 'Parallel' in algo else 'lightcoral' for algo in avg_times.index]
        
        bars = ax.barh(range(len(avg_times)), avg_times.values, color=colors)
        ax.set_yticks(range(len(avg_times)))
        ax.set_yticklabels(avg_times.index)
        ax.set_xlabel('Average Time (ms)')
        ax.set_title('Overall Performance')
        
        for i, v in enumerate(avg_times.values):
            ax.text(v + max(avg_times.values)*0.01, i, f'{v:.1f}ms', va='center')
    
    def _plot_scalability(self, ax):
        for algo in ['Prim_BinaryHeap', 'KKT', 'Boruvka_Parallel_4threads']:
            if algo in self.df['Algorithm'].values:
                algo_data = self.df[self.df['Algorithm'] == algo]
                avg_by_size = algo_data.groupby('Vertices')['Time(ms)'].mean()
                ax.plot(avg_by_size.index, avg_by_size.values, 'o-', label=algo, markersize=4)
        
        ax.set_xlabel('Vertices')
        ax.set_ylabel('Time (ms)')
        ax.set_title('Scalability')
        ax.legend()
        ax.set_yscale('log')
    
    def _plot_density_performance(self, ax):
        medium_graphs = self.df[self.df['Vertices'] >= 1000]
        for algo in ['Prim_BinaryHeap', 'KKT']:
            if algo in medium_graphs['Algorithm'].values:
                algo_data = medium_graphs[medium_graphs['Algorithm'] == algo]
                avg_by_density = algo_data.groupby('Density')['Time(ms)'].mean()
                ax.plot(avg_by_density.index, avg_by_density.values, 's-', label=algo, markersize=4)
        
        ax.set_xlabel('Density')
        ax.set_ylabel('Time (ms)')
        ax.set_title('Performance vs Density (V≥1000)')
        ax.legend()
        ax.set_xscale('log')
        ax.set_yscale('log')
    
    def _plot_algorithm_ranking(self, ax):
        pivot_data = self.df.pivot_table(values='Time(ms)', index='Vertices', 
                                       columns='Algorithm', aggfunc='mean')
        normalized = pivot_data.div(pivot_data.min(axis=1), axis=0)
        
        im = ax.imshow(normalized.values, cmap='RdYlGn_r', aspect='auto', vmin=1, vmax=5)
        ax.set_xticks(range(len(normalized.columns)))
        ax.set_xticklabels(normalized.columns, rotation=45, ha='right')
        ax.set_yticks(range(len(normalized.index)))
        ax.set_yticklabels([f'V{v}' for v in normalized.index])
        ax.set_title('Performance Ranking\n(1.0 = fastest)')
        plt.colorbar(im, ax=ax)
    

if __name__ == "__main__":
    analyzer = SimpleResultAnalyzer()
    if analyzer.load_data():
        analyzer.create_simple_report()