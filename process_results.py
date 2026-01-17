#!/usr/bin/env python3
"""
Script to process resultados.csv and generate best_results.csv and average_results.csv
"""

import csv
import sys
import os
from collections import defaultdict
from typing import Dict, List, Optional, Tuple


def parse_float(value: str) -> Optional[float]:
    """Parse a string to float, returning None if empty or invalid."""
    if not value or value.strip() == '':
        return None
    try:
        return float(value)
    except ValueError:
        return None


def calculate_deviation(result: float, ideal: Optional[float]) -> Optional[float]:
    """Calculate percentage deviation: ((result - ideal) / ideal) * 100"""
    if ideal is None or ideal == 0:
        return None
    return ((result - ideal) / ideal) * 100


def read_optimal_from_instance(instance_name: str, instances_dir: str = 'capmstnew') -> Optional[float]:
    """
    Read the instance file and check if the last non-empty line has exactly one number.
    Returns that number if it's a single number, None otherwise.
    """
    instance_path = os.path.join(instances_dir, instance_name)
    
    if not os.path.exists(instance_path):
        return None
    
    try:
        with open(instance_path, 'r', encoding='utf-8') as f:
            lines = f.readlines()
        
        # Find the last non-empty line
        last_line = None
        for line in reversed(lines):
            stripped = line.strip()
            if stripped:
                last_line = stripped
                break
        
        if last_line is None:
            return None
        
        # Split by whitespace and check if there's exactly one token
        tokens = last_line.split()
        
        # If there's exactly one token, try to parse it as a number
        if len(tokens) == 1:
            try:
                return float(tokens[0])
            except ValueError:
                return None
        
        # If there's more than one token, it's not a resultado_ideal
        return None
        
    except Exception:
        return None


def process_results(input_file: str, instances_dir: str = 'capmstnew'):
    """Process resultados.csv and generate best_results.csv and average_results.csv"""
    
    # Data structures to store results
    # Key: instance name
    # Value: dict with algorithm/alpha combinations
    best_data = defaultdict(lambda: {
        'resultado_ideal': None,
        'guloso': [],
        'randomizado_0.03': [],
        'randomizado_0.04': [],
        'randomizado_0.05': [],
        'reativo': []
    })
    
    avg_data = defaultdict(lambda: {
        'resultado_ideal': None,
        'guloso': {'solutions': [], 'times': []},
        'randomizado_0.03': {'solutions': [], 'times': []},
        'randomizado_0.04': {'solutions': [], 'times': []},
        'randomizado_0.05': {'solutions': [], 'times': []},
        'reativo': {'solutions': [], 'times': []}
    })
    
    # Set to track which instances we've seen
    seen_instances = set()
    
    # Read input CSV
    with open(input_file, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        
        for row in reader:
            instancia = row['instancia']
            algoritmo = row['algoritmo']
            alpha_str = row['alpha']
            melhor_solucao = parse_float(row['melhor_solucao'])
            tempo_segundos = parse_float(row['tempo_segundos'])
            
            # Read resultado_ideal from instance file (only once per instance)
            if instancia not in seen_instances:
                seen_instances.add(instancia)
                resultado_ideal = read_optimal_from_instance(instancia, instances_dir)
                best_data[instancia]['resultado_ideal'] = resultado_ideal
                avg_data[instancia]['resultado_ideal'] = resultado_ideal
            
            if melhor_solucao is None:
                continue
            
            # Categorize by algorithm and alpha
            if algoritmo == 'guloso':
                best_data[instancia]['guloso'].append(melhor_solucao)
                if tempo_segundos is not None:
                    avg_data[instancia]['guloso']['solutions'].append(melhor_solucao)
                    avg_data[instancia]['guloso']['times'].append(tempo_segundos)
            
            elif algoritmo == 'randomizado':
                if alpha_str:
                    alpha = float(alpha_str)
                    if alpha == 0.03:
                        best_data[instancia]['randomizado_0.03'].append(melhor_solucao)
                        if tempo_segundos is not None:
                            avg_data[instancia]['randomizado_0.03']['solutions'].append(melhor_solucao)
                            avg_data[instancia]['randomizado_0.03']['times'].append(tempo_segundos)
                    elif alpha == 0.04:
                        best_data[instancia]['randomizado_0.04'].append(melhor_solucao)
                        if tempo_segundos is not None:
                            avg_data[instancia]['randomizado_0.04']['solutions'].append(melhor_solucao)
                            avg_data[instancia]['randomizado_0.04']['times'].append(tempo_segundos)
                    elif alpha == 0.05:
                        best_data[instancia]['randomizado_0.05'].append(melhor_solucao)
                        if tempo_segundos is not None:
                            avg_data[instancia]['randomizado_0.05']['solutions'].append(melhor_solucao)
                            avg_data[instancia]['randomizado_0.05']['times'].append(tempo_segundos)
            
            elif algoritmo == 'reativo':
                best_data[instancia]['reativo'].append(melhor_solucao)
                if tempo_segundos is not None:
                    avg_data[instancia]['reativo']['solutions'].append(melhor_solucao)
                    avg_data[instancia]['reativo']['times'].append(tempo_segundos)
    
    # Generate best_results.csv
    with open('best_results.csv', 'w', newline='', encoding='utf-8') as f:
        fieldnames = [
            'instancia', 'resultado_ideal',
            'guloso', 'desvio_guloso',
            'randomizado_alfa_0.03', 'desvio_randomizado_alfa_0.03',
            'randomizado_alfa_0.04', 'desvio_randomizado_alfa_0.04',
            'randomizado_alfa_0.05', 'desvio_randomizado_alfa_0.05',
            'reativo', 'desvio_reativo'
        ]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        
        for instancia in sorted(best_data.keys()):
            data = best_data[instancia]
            ideal = data['resultado_ideal']
            
            # Find best (minimum) for each algorithm
            best_guloso = min(data['guloso']) if data['guloso'] else None
            best_rand_003 = min(data['randomizado_0.03']) if data['randomizado_0.03'] else None
            best_rand_004 = min(data['randomizado_0.04']) if data['randomizado_0.04'] else None
            best_rand_005 = min(data['randomizado_0.05']) if data['randomizado_0.05'] else None
            best_reativo = min(data['reativo']) if data['reativo'] else None
            
            row = {
                'instancia': instancia,
                'resultado_ideal': f'{ideal:.2f}' if ideal is not None else '',
                'guloso': f'{best_guloso:.2f}' if best_guloso is not None else '',
                'desvio_guloso': f'{calculate_deviation(best_guloso, ideal):.2f}' if best_guloso is not None and ideal is not None else '',
                'randomizado_alfa_0.03': f'{best_rand_003:.2f}' if best_rand_003 is not None else '',
                'desvio_randomizado_alfa_0.03': f'{calculate_deviation(best_rand_003, ideal):.2f}' if best_rand_003 is not None and ideal is not None else '',
                'randomizado_alfa_0.04': f'{best_rand_004:.2f}' if best_rand_004 is not None else '',
                'desvio_randomizado_alfa_0.04': f'{calculate_deviation(best_rand_004, ideal):.2f}' if best_rand_004 is not None and ideal is not None else '',
                'randomizado_alfa_0.05': f'{best_rand_005:.2f}' if best_rand_005 is not None else '',
                'desvio_randomizado_alfa_0.05': f'{calculate_deviation(best_rand_005, ideal):.2f}' if best_rand_005 is not None and ideal is not None else '',
                'reativo': f'{best_reativo:.2f}' if best_reativo is not None else '',
                'desvio_reativo': f'{calculate_deviation(best_reativo, ideal):.2f}' if best_reativo is not None and ideal is not None else ''
            }
            writer.writerow(row)
    
    # Generate average_results.csv
    with open('average_results.csv', 'w', newline='', encoding='utf-8') as f:
        fieldnames = [
            'instancia', 'resultado_ideal',
            'guloso', 'desvio_guloso', 'tempo_guloso',
            'randomizado_alfa_0.03', 'desvio_randomizado_alfa_0.03', 'tempo_randomizado_alfa_0.03',
            'randomizado_alfa_0.04', 'desvio_randomizado_alfa_0.04', 'tempo_randomizado_alfa_0.04',
            'randomizado_alfa_0.05', 'desvio_randomizado_alfa_0.05', 'tempo_randomizado_alfa_0.05',
            'reativo', 'desvio_reativo', 'tempo_reativo'
        ]
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        
        for instancia in sorted(avg_data.keys()):
            data = avg_data[instancia]
            ideal = data['resultado_ideal']
            
            # Calculate averages
            def avg(values):
                return sum(values) / len(values) if values else None
            
            avg_guloso = avg(data['guloso']['solutions'])
            avg_tempo_guloso = avg(data['guloso']['times'])
            avg_rand_003 = avg(data['randomizado_0.03']['solutions'])
            avg_tempo_rand_003 = avg(data['randomizado_0.03']['times'])
            avg_rand_004 = avg(data['randomizado_0.04']['solutions'])
            avg_tempo_rand_004 = avg(data['randomizado_0.04']['times'])
            avg_rand_005 = avg(data['randomizado_0.05']['solutions'])
            avg_tempo_rand_005 = avg(data['randomizado_0.05']['times'])
            avg_reativo = avg(data['reativo']['solutions'])
            avg_tempo_reativo = avg(data['reativo']['times'])
            
            row = {
                'instancia': instancia,
                'resultado_ideal': f'{ideal:.2f}' if ideal is not None else '',
                'guloso': f'{avg_guloso:.2f}' if avg_guloso is not None else '',
                'desvio_guloso': f'{calculate_deviation(avg_guloso, ideal):.2f}' if avg_guloso is not None and ideal is not None else '',
                'tempo_guloso': f'{avg_tempo_guloso:.6f}' if avg_tempo_guloso is not None else '',
                'randomizado_alfa_0.03': f'{avg_rand_003:.2f}' if avg_rand_003 is not None else '',
                'desvio_randomizado_alfa_0.03': f'{calculate_deviation(avg_rand_003, ideal):.2f}' if avg_rand_003 is not None and ideal is not None else '',
                'tempo_randomizado_alfa_0.03': f'{avg_tempo_rand_003:.6f}' if avg_tempo_rand_003 is not None else '',
                'randomizado_alfa_0.04': f'{avg_rand_004:.2f}' if avg_rand_004 is not None else '',
                'desvio_randomizado_alfa_0.04': f'{calculate_deviation(avg_rand_004, ideal):.2f}' if avg_rand_004 is not None and ideal is not None else '',
                'tempo_randomizado_alfa_0.04': f'{avg_tempo_rand_004:.6f}' if avg_tempo_rand_004 is not None else '',
                'randomizado_alfa_0.05': f'{avg_rand_005:.2f}' if avg_rand_005 is not None else '',
                'desvio_randomizado_alfa_0.05': f'{calculate_deviation(avg_rand_005, ideal):.2f}' if avg_rand_005 is not None and ideal is not None else '',
                'tempo_randomizado_alfa_0.05': f'{avg_tempo_rand_005:.6f}' if avg_tempo_rand_005 is not None else '',
                'reativo': f'{avg_reativo:.2f}' if avg_reativo is not None else '',
                'desvio_reativo': f'{calculate_deviation(avg_reativo, ideal):.2f}' if avg_reativo is not None and ideal is not None else '',
                'tempo_reativo': f'{avg_tempo_reativo:.6f}' if avg_tempo_reativo is not None else ''
            }
            writer.writerow(row)
    
    print(f"Successfully generated best_results.csv and average_results.csv from {input_file}")


if __name__ == '__main__':
    input_file = 'resultados.csv'
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    
    try:
        process_results(input_file)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing file: {e}")
        sys.exit(1)

