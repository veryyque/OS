#!/usr/bin/env python3
"""
Анализатор дампа стека shared_stack.bin
"""

import struct

def analyze_stack_dump(filename="shared_stack.bin"):
    
    try:
        with open(filename, 'rb') as f:
            data = f.read()
    except FileNotFoundError:
        print("Файл {} не найден! Сначала запустите программу на C.".format(filename))
        return
    
    print("=" * 60)
    print("Анализ файла: {}".format(filename))
    print("Размер файла: {} байт".format(len(data)))
    print("=" * 60)
    print()
    
    # 1. ПОИСК СТРОК "hello world"
    print("1. ПОИСК СТРОК 'hello world'")
    print("-" * 40)
    
    hello_world = b"hello world"
    positions = []
    
    start = 0
    while True:
        pos = data.find(hello_world, start)
        if pos == -1:
            break
        positions.append(pos)
        start = pos + 1
    
    print("Найдено строк 'hello world': {}".format(len(positions)))
    print("Ожидалось: 10 (глубина рекурсии)")
    print()
    
    for i, pos in enumerate(positions, 1):
        print("  {}. Смещение {} (0x{:04x})".format(i, pos, pos))
    
    if len(positions) == 10:
        print("Совпадает с ожидаемым")
    else:
        print("Несоответствие! Найдено {}, ожидалось 10".format(len(positions)))
    
    print()
    print("=" * 60)
    print()
    
    # 2. ПОИСК ПЕРЕМЕННОЙ depth
    print("2. ПОИСК ПЕРЕМЕННОЙ depth (счётчик рекурсии)")
    print("-" * 40)
    
    found_depths = {}
    for depth in range(11):
        found_depths[depth] = []
    
    for i in range(len(data) - 3):
        value = struct.unpack('<I', data[i:i+4])[0]
        if 0 <= value <= 10:
            found_depths[value].append(i)
    
    print("Найденные значения depth:")
    for depth in range(10, -1, -1):
        count = len(found_depths[depth])
        if count > 0:
            print("  depth = {}: найдено {} раз".format(depth, count))
            for pos in found_depths[depth][:2]:
                print("      смещение: 0x{:04x}".format(pos))
    
    print()
    print("=" * 60)
    print()
    
    # 3. ПОИСК АДРЕСОВ ВОЗВРАТА
    print("3. ПОИСК АДРЕСОВ ВОЗВРАТА")
    print("-" * 40)
    
    return_addresses = []
    
    for i in range(len(data) - 7):
        addr = struct.unpack('<Q', data[i:i+8])[0]
        if 0x400000 < addr < 0x7fffffffffff:
            if addr % 4 == 0:
                return_addresses.append((i, addr))
    
    print("Найдено потенциальных адресов возврата: {}".format(len(return_addresses)))
    
    unique_addrs = {}
    for pos, addr in return_addresses:
        if addr not in unique_addrs:
            unique_addrs[addr] = []
        unique_addrs[addr].append(pos)
    
    print("Уникальных адресов: {}".format(len(unique_addrs)))
    print()
    print("Наиболее частые адреса (это адреса возврата):")
    
    sorted_addrs = sorted(unique_addrs.items(), key=lambda x: -len(x[1]))[:10]
    
    for addr, positions_list in sorted_addrs:
        count = len(positions_list)
        addr_hex = hex(addr)
        print("  {} встречается {} раз(а)".format(addr_hex, count))
        if count <= 3:
            for pos in positions_list:
                print("      на смещении 0x{:04x}".format(pos))
        else:
            print("      на смещениях: 0x{:04x}, 0x{:04x}, ...".format(
                positions_list[0], positions_list[1]))
    
    print()
    print("=" * 60)
    print()
    
    # 4. ВЫВОД ОКОЛО ПОСЛЕДНЕЙ СТРОКИ
    if positions:
        last_hello = max(positions)
        print("4. ДАМП ВОКРУГ ПОСЛЕДНЕЙ СТРОКИ 'hello world'")
        print("-" * 40)
        print("Смещение последней строки: 0x{:04x}".format(last_hello))
        print()
        
        start = max(0, last_hello - 16)
        end = min(len(data), last_hello + 48)
        
        for offset in range(start, end, 8):
            chunk = data[offset:offset+8]
            hex_str = ' '.join('{:02x}'.format(b) for b in chunk)
            ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in chunk)
            marker = " <-- hello" if offset <= last_hello < offset+8 else ""
            print("0x{:04x}: {:<24} {}{}".format(offset, hex_str, ascii_str, marker))
    
    print()
    print("=" * 60)
    print("Анализ завершён")
    print("=" * 60)


if __name__ == "__main__":
    analyze_stack_dump("shared_stack.bin")