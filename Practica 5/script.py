import sys

if len(sys.argv) < 2:
    print("Uso: python convert_obj.py archivo.obj")
    sys.exit()

archivo = sys.argv[1]

vertices = []
indices = []

with open(archivo, "r") as f:
    for linea in f:
        linea = linea.strip()

        # VÉRTICES
        if linea.startswith("v "):
            _, x, y, z = linea.split()
            vertices.append((float(x), float(y), float(z)))

        # CARAS
        elif linea.startswith("f "):
            partes = linea.split()[1:]  # quitar el 'f'

            for p in partes:
                # Formato posible:  "7//66"  o "7/3/66"  o "7"
                num = p.split('/')[0]
                idx = int(num) - 1  # OBJ usa base-1
                indices.append(idx)

# ---- SALIDA ----

print("\n=== VÉRTICES (cb::Vec3) ===\n")
for v in vertices:
    print(f"vertices.insert(vertices.end(), cb::Vec3({v[0]}, {v[1]}, {v[2]}));")

print("\n=== ÍNDICES (GLuint) ===\n")
for i in range(0, len(indices), 3):
    a = indices[i]
    b = indices[i+1]
    c = indices[i+2]
    print(f"indices.push_back({a});") 
    print(f"indices.push_back({b});")
    print(f"indices.push_back({c});")