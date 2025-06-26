# Scripts

Run scripts from the root directory.

## extract-cpp

Extracts contents of all fenced code blocks with no preceeding whitespace
labeled `cpp`. Each code block is emitted to a separate directory. The directory
is named after the source Markdown file, with the initial `src/` removed, with
`listings/cc/` prepended and with a number appended.

E.g., if the following Markdown file is at `src/foo/bar/baz.md`

``````markdown
# Title

```cpp
one
```

```rust
two
```

```cpp
three
```
``````

it will produce the file `listings/cc/foo/bar/baz-0/example.cc` containing

```text
one
```

and the file `listings/cc/foo/bar/baz-1/example.cc` containing the file

```text
three
```

Makes use of `extract.hs` which requires a GHC Haskell installation and `runghc`
on the path.
