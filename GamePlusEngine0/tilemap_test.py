import xml.etree.ElementTree as ET

def parse_tmx_file(file_path):
    tree = ET.parse(file_path)
    root = tree.getroot()

    # Assuming there is only one tileset for simplicity. Adjust as needed.
    tileset = root.find(".//tileset")

    tile_properties = {}

    for tile in tileset.findall(".//tile"):
        tile_id = int(tile.attrib["id"])

        properties = {}
        for prop in tile.findall(".//properties/property"):
            prop_name = prop.attrib["name"]
            prop_value = prop.attrib["value"]
            properties[prop_name] = prop_value

        tile_properties[tile_id] = properties

    return tile_properties

# Example usage
tmx_file_path = "./data/untitled.tmx"
properties = parse_tmx_file(tmx_file_path)

# Now, 'properties' is a dictionary where keys are tile IDs and values are dictionaries of tile properties.
print(properties)