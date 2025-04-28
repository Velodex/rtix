# Contributing

Reach out to Parker (owner) if you'd like to contribute.

## New PR checklist
- [ ] Is the code change documented?
- [ ] Are there unit tests for the code change?
- [ ] Does the code change follow the style guidelines?
- [ ] Has the code been properly formatted?
- [ ] Has the code changed been reviewed?

## Style guide
- Use the provided .clang-format for formatting.  Recommended to use vscode for development.
- `snake_case` for variables
- `_` prefix for private class members (omit for POD structs), e.g.
    - `_privateMethod()`
    - `_private_var`
- `PascalCase` for class names and static methods
- `camelCase` for methods
- Begin methods with a verb `doSomething`
- Return one value at the function output.  If multiple values are needed, return a struct or relevant container, or pass a reference to the function
- Use exceptions in libraries, handle in integration layer

## Checklist to update a version
1. Update the [CHANGELOG.md](CHANGELOG.md) with relevent updates for the new version.
2. The version needs to be updated in two places: the `version.txt` and `setup.py`.
3. Create a pull request to review the new version
4. After the pull request merges, update the tag and push
```bash
git tag -a v0.0.0 -m "Version 0.0.0"
git push origin v0.0.0
```
5. Package for Pypi below.

## Packaging for PyPi
Based on the [Python instructions](https://packaging.python.org/en/latest/tutorials/packaging-projects/).

Inside, the Docker container, build the package distribution, including the generated protobuf files
```bash
python3 -m setup.py generate_protos
python3 -m build
```

Upload to twine.  Note that you will need a PyPi account and credentials for the rtix package for this to work.
```bash
python3 -m twine upload dist/*
```
