#!/usr/bin/env python3
"""
TPC-H-like Data Generator for NanoDB
Generates synthetic data matching TPC-H schema without requiring official license.
"""

import random
import os

def generate_customers(num_rows=20000):
    """Generate customers.tbl with TPC-H-like schema."""
    segments = ["BUILDING", "AUTOMOBILE", "MACHINERY", "HOUSEHOLD", "FURNITURE"]
    first_names = ["Alice", "Bob", "Charlie", "Diana", "Eve", "Frank", "Grace", "Henry", 
                   "Ivy", "Jack", "Kate", "Leo", "Mike", "Nina", "Oscar", "Paul", 
                   "Quinn", "Rose", "Sam", "Tina", "Uma", "Victor", "Wendy", "Xavier", "Yara", "Zack"]
    last_names = ["Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller", 
                  "Davis", "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez", 
                  "Wilson", "Anderson", "Thomas", "Taylor", "Moore", "Jackson", "Martin"]
    
    output_dir = "data"
    os.makedirs(output_dir, exist_ok=True)
    
    with open(os.path.join(output_dir, "customers.tbl"), "w") as f:
        for i in range(1, num_rows + 1):
            c_custkey = i
            c_name = f"{random.choice(first_names)}{random.choice(last_names)}{i:04d}"
            c_acctbal = round(random.uniform(1000.0, 15000.0), 2)
            c_mktsegment = random.choice(segments)
            c_nationkey = random.randint(0, 24)
            
            f.write(f"{c_custkey}|{c_name}|{c_acctbal}|{c_mktsegment}|{c_nationkey}\n")
    
    print(f"Generated {num_rows} customer records in data/customers.tbl")

def generate_orders(num_rows=30000, customer_count=20000):
    """Generate orders.tbl with TPC-H-like schema."""
    statuses = ["O", "F", "P"]
    
    output_dir = "data"
    os.makedirs(output_dir, exist_ok=True)
    
    with open(os.path.join(output_dir, "orders.tbl"), "w") as f:
        for i in range(1, num_rows + 1):
            o_orderkey = i
            o_custkey = random.randint(1, customer_count)
            o_orderstatus = random.choice(statuses)
            o_totalprice = round(random.uniform(50.0, 50000.0), 2)
            
            f.write(f"{o_orderkey}|{o_custkey}|{o_orderstatus}|{o_totalprice}\n")
    
    print(f"Generated {num_rows} order records in data/orders.tbl")

def generate_lineitems(num_rows=50000, order_count=30000):
    """Generate lineitem.tbl with TPC-H-like schema."""
    
    output_dir = "data"
    os.makedirs(output_dir, exist_ok=True)
    
    with open(os.path.join(output_dir, "lineitem.tbl"), "w") as f:
        for i in range(1, num_rows + 1):
            l_orderkey = random.randint(1, order_count)
            l_partkey = random.randint(1, 200000)
            l_quantity = round(random.uniform(1.0, 50.0), 2)
            l_extendedprice = round(random.uniform(10.0, 10000.0), 2)
            
            f.write(f"{l_orderkey}|{l_partkey}|{l_quantity}|{l_extendedprice}\n")
    
    print(f"Generated {num_rows} lineitem records in data/lineitem.tbl")

def main():
    """Generate all TPC-H-like data files."""
    print("Generating TPC-H-like data for NanoDB...")
    
    generate_customers(20000)
    generate_orders(30000, 20000)
    generate_lineitems(50000, 30000)
    
    print("Data generation complete!")

if __name__ == "__main__":
    random.seed(42)  # For reproducibility
    main()
