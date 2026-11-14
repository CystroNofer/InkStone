# InkStone design wiki

[Project home](../README.md)

Baseline: the working tree inspected on **2026-10-24**, including the current
Assimp importer. This describes code on disk, not necessarily the latest commit.

## Read by topic

| Page | Questions it answers |
| --- | --- |
| [Architecture](Architecture.md) | Which module does what? What runs each frame? Who owns resources? |
| [Scene and hierarchy](Scene.md) | How are entities created, queried, parented, and destroyed? |
| [Assets and model loading](Assets.md) | What does import retain? How does a model become scene entities? |
| [Rendering and UI](RenderingAndUI.md) | How do meshes, materials, shaders, and the hierarchy widget interact? |
| [Building](Building.md) | How do I generate, build, and run the application? |
| [Decisions and backlog](Decisions.md) | Why were these choices made, and what remains unresolved? |

For a first pass, read Architecture, Scene, and Assets in that order. For setup,
go directly to Building.

## Status vocabulary

- **Implemented** describes behavior present in the linked source.
- **Known limitation** describes a missing capability or a concrete gap in that behavior.
- **Proposed** describes future work, not an API or guarantee that exists today.

Source links are included on each page. When code and documentation disagree,
inspect the code and update the relevant page in the same change. Do not document
a planned feature as implemented merely because a field or stub exists.

## Maintaining the wiki

- Keep design contracts here; keep short implementation explanations beside code.
- Update ownership, lifetime, failure behavior, and call ordering when they change.
- Record significant choices in [Decisions](Decisions.md), without turning it into
  a chronological list of every edit.
- Use relative Markdown links so navigation works in the repository and on GitHub.
- Avoid duplicating full class declarations. Link to the declaration instead.
- Do not describe temporary local checks as committed test commands.

