#!/usr/bin/env python3
"""
migrate_animals_csv.py

Usage:
  - Export Google Sheets sheets to CSV files (e.g., flock.csv, herd.csv).
  - Run this script to convert CSV rows into the new fixed order expected by the codebase:
    RFID,Tag_Colour,Visual_ID,Breed,Type,Name,Tagged,Gender,Dam,Sire,Multi_birth,Location,Group,Status,WHP,Comment

Examples (PowerShell):
  python .\tools\migrate_animals_csv.py C:\path\to\flock.csv
  python .\tools\migrate_animals_csv.py -d C:\path\to\exports\

The script makes a timestamped backup of each original file before replacing it.
It is header-aware: it will attempt to locate columns by common names and fill missing columns with blanks.
"""

import csv
import os
import shutil
import argparse
from datetime import datetime

TARGET_COLS = [
    'RFID', 'Tag_Colour', 'Visual_ID', 'Breed', 'Type', 'Name', 'Tagged', 'Gender',
    'Dam', 'Sire', 'Multi_birth', 'Location', 'Group', 'Status', 'WHP', 'Comment'
]

# Candidate header keywords mapped to canonical target column index
CANDIDATES = {
    'rfid': 'RFID',
    'tag_colour': 'Tag_Colour',
    'tag colour': 'Tag_Colour',
    'colour': 'Tag_Colour',
    'color': 'Tag_Colour',
    'visual_id': 'Visual_ID',
    'visual id': 'Visual_ID',
    'visual': 'Visual_ID',
    'breed': 'Breed',
    'type': 'Type',
    'name': 'Name',
    'tagged': 'Tagged',
    'date tagged': 'Tagged',
    'gender': 'Gender',
    'dam': 'Dam',
    'mother': 'Dam',
    'sire': 'Sire',
    'father': 'Sire',
    'multi': 'Multi_birth',
    'multi_birth': 'Multi_birth',
    'multiple': 'Multi_birth',
    'location': 'Location',
    'group': 'Group',
    'status': 'Status',
    'whp': 'WHP',
    'whpSafeDate': 'WHP',
    'comment': 'Comment',
    'notes': 'Comment'
}


def normalize(hdr):
    return hdr.strip().strip('"').strip().lower()


def map_headers(headers):
    """Return a dict canonical_col -> index or -1 if not found."""
    hmap = {}
    lower_headers = [normalize(h) for h in headers]
    for col in TARGET_COLS:
        hmap[col] = -1
    for idx, h in enumerate(lower_headers):
        if h == '':
            continue
        # direct match
        if h.upper() in TARGET_COLS:
            hmap[h.upper()] = idx
            continue
        # candidate mapping
        if h in CANDIDATES:
            canon = CANDIDATES[h]
            hmap[canon] = idx
            continue
        # try contains matches
        for key, canon in CANDIDATES.items():
            if key in h:
                hmap[canon] = idx
                break
    return hmap


def process_file(path, in_place=True):
    print(f"Processing: {path}")
    if not os.path.isfile(path):
        print("  Not a file, skipping")
        return

    # read all rows
    with open(path, newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        rows = list(reader)

    if len(rows) == 0:
        print("  Empty file, skipping")
        return

    header = rows[0]
    mapping = map_headers(header)

    # If header already matches canonical ordering, still rewrite to standardized header
    already_good = ([normalize(h) for h in header][:len(TARGET_COLS)] == [c.lower() for c in TARGET_COLS])

    timestamp = datetime.now().strftime('%Y%m%d%H%M%S')
    backup_path = path + f'.bak.{timestamp}'
    shutil.copy2(path, backup_path)
    print(f"  Backup created: {backup_path}")

    temp_path = path + '.tmp'
    with open(temp_path, 'w', newline='', encoding='utf-8') as out_f:
        writer = csv.writer(out_f)
        writer.writerow(TARGET_COLS)
        for row in rows[1:]:
            new_row = []
            for col in TARGET_COLS:
                src_idx = mapping.get(col, -1)
                if src_idx != -1 and src_idx < len(row):
                    new_row.append(row[src_idx])
                else:
                    new_row.append('')
            writer.writerow(new_row)

    # Replace original
    shutil.move(temp_path, path)
    print(f"  Rewrote file with canonical header. Original backed up at {backup_path}")


def process_directory(dirpath):
    for entry in os.listdir(dirpath):
        full = os.path.join(dirpath, entry)
        if os.path.isfile(full) and entry.lower().endswith('.csv'):
            # skip tags.csv unless user explicitly wants it
            if entry.lower() == 'tags.csv':
                print(f"Skipping tags.csv (not modified): {full}")
                continue
            process_file(full)
        elif os.path.isdir(full):
            # look for species folders (Sheep, Cattle etc.)
            # process Flock.csv or Herd.csv inside
            for subentry in os.listdir(full):
                subfull = os.path.join(full, subentry)
                if os.path.isfile(subfull) and subentry.lower().endswith('.csv'):
                    if subentry.lower() == 'tags.csv':
                        print(f"Skipping tags.csv (not modified): {subfull}")
                        continue
                    process_file(subfull)


def main():
    parser = argparse.ArgumentParser(description='Migrate animal CSVs to new column order')
    parser.add_argument('paths', nargs='*', help='CSV files or directories to process')
    parser.add_argument('-d', '--dir', help='directory containing exported CSV files (recursive on species folders)')
    args = parser.parse_args()

    targets = []
    if args.dir:
        targets.append(args.dir)
    if args.paths:
        targets.extend(args.paths)
    if not targets:
        print('No files or directories specified. Use -h for help.')
        return

    for t in targets:
        if os.path.isdir(t):
            process_directory(t)
        elif os.path.isfile(t):
            process_file(t)
        else:
            print(f"Path not found: {t}")


if __name__ == '__main__':
    main()
