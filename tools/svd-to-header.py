import re
import xml.etree.ElementTree as ET

def process(filepath):
    document = ET.parse(filepath)
    root = document.getroot()
    output = {}
    for peripheral in root.findall(".//peripheral"):
        output |= process_peripheral(peripheral)
    return output

def process_peripheral(peripheral):
    output = {}
    name = peripheral.find("name").text.upper()
    baseAddress = peripheral.find("baseAddress").text.upper()
    output[f"{name}_BASE"] = baseAddress
    for register in peripheral.findall(".//register"):
        output |= process_register(register, name)
    return output

def process_register(register, basename):
    output = {}
    name = f"{basename}_{register.find('name').text}".upper()
    offset = register.find("addressOffset").text.upper()
    output[f"{name}_OFFSET"] = offset
    for field in register.findall(".//field"):
        output |= process_field(field, name)
    return output

def process_field(field, basename):
    output = {}
    name = f"{basename}_{field.find('name').text}".upper()
    bitRange = field.find('bitRange')
    range = bitRange.text if bitRange is not None else "[31:0]"
    output[f"{name}_MASK"] = bitmask(range)
    output[f"{name}_OFFSET"] = bitoffset(range)
    for enum in field.findall(".//enumeratedValue"):
        output |= process_enum(enum, name)
    return output

def bitmask(range):
    hi, lo = map(int, re.findall(r'\d+', range))
    width = hi - lo + 1
    return hex(((1 << width) - 1) << lo).upper()

def bitoffset(range):
    hi, lo = map(int, re.findall(r"\d+", range))
    return lo

def process_enum(enum, basename):
    output = {}
    name = f"{basename}_{enum.find('name').text}".upper()
    value = enum.find('value').text
    output[f"{name}"] = value
    return output

def generate_header_file(definitions):
    with open("../rp2040.h", "w") as f:
        f.write("#pragma once\n")
        f.write("\n")
        for k, v in definitions.items():
            f.write(f"#define {k} {v}\n")

def main():
    definitions = process("../rp2040.svd")
    generate_header_file(definitions)

if __name__ == "__main__":
    main()