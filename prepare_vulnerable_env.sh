#!/bin/bash

# Script per compilare tutti i file vulnerabili con le difese disattivate
# Preparazione per gli exploit proof-of-concept

mkdir -p build

echo "[*] Compilazione in corso dei file vulnerabili..."

# 1. Stack Buffer Overflow
echo "1. Compilando: CWE-787 Stack Buffer Overflow..."
gcc -fno-stack-protector -z execstack -no-pie -g -o "build/CWE-787 Stack Buffer Overflow" "vulnerabili/CWE-787 Stack Buffer Overflow.c"

# 2. Heap Buffer Overflow
echo "2. Compilando: CWE-787 Heap Buffer Overflow..."
gcc -fno-stack-protector -z execstack -no-pie -g -o "build/CWE-787 Heap Buffer Overflow" "vulnerabili/CWE-787 Heap Buffer Overflow.c"

# 3. Array Index OOB
echo "3. Compilando: CWE-787 Array Index OOB..."
gcc -fno-stack-protector -z execstack -no-pie -g -o "build/CWE-787 Array Index OOB" "vulnerabili/CWE-787 Array Index OOB.c"

# 4. Off-by-one Error
echo "4. Compilando: CWE-787 Off-by-one Error..."
gcc -fno-stack-protector -z execstack -no-pie -g -o "build/CWE-787 Off-by-one Error" "vulnerabili/CWE-787 Off-by-one Error.c"

# 5. Pointer Arithmetic OOB
echo "5. Compilando: CWE-787 Pointer Arithmetic OOB..."
gcc -fno-stack-protector -z execstack -no-pie -g -o "build/CWE-787 Pointer Arithmetic OOB" "vulnerabili/CWE-787 Pointer Arithmetic OOB.c"

echo ""
echo "[*] Compilazione completata! I file eseguibili si trovano nella cartella 'build/'."
echo "    Puoi eseguire gli exploit dalla root del progetto così:"
echo "    python3 exploits/Exploit_Stack_Buffer_Overflow.py"
