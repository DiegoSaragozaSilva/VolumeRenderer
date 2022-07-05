for file in ./src/shaders/*
do
    if [[ $file != *".spv"* ]]; then
        name="${file%.*}.spv"
        glslc "$file" -o $name
    fi
done
