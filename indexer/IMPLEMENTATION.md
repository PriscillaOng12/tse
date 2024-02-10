# CS50 TSE Indexer
## Implementation Spec

## Overview

This document elaborates on the specifics of implementing the TSE Indexer, grounded in the framework provided by the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md). 

## Data Structures Overview

The indexer relies on a custom `index` structure, essentially a hashtable that maps words (strings) to `counter` objects. This `index` is implemented in `index.c` and is pivotal for correlating words with their occurrences in various documents identified by docID.

## Algorithmic Flow

The indexer's logic is encapsulated within `indexer.c`, with two static functions (`indexBuild` and `indexPage`), and interfaces with `index.c` through `index_new`, `index_save`, `index_delete`, and `index_add`. An auxiliary `indextest.c` file supports testing by leveraging `index_new`, `index_load`, `index_save`, and `index_delete`.

### Primary Operations

- **indexer.c**: Main indexing process, invoking `index_new`, `indexBuild`, `index_save`, and `index_delete` sequentially.
- **indextest.c**: Facilitates testing through a sequence of `index_new`, `index_load`, `index_save`, and `index_delete` operations.

### Key Functions in indexer.c

- **indexBuild**: Processes each document within a given directory, leveraging `indexPage` to parse and index HTML content.
- **indexPage**: Analyzes HTML, updating the index with word occurrences.

## Module Interactions

- **pagedir**: Utilizes common library modules for managing indexed data.
- **libcs50**: Draws on `hashtable`, `counters`, and `webpage` modules for foundational data handling.

## Error Management

We take a defensive programming stance, ensuring all inputs are validated upfront to avoid execution with invalid parameters. Out-of-memory scenarios are acknowledged but not explicitly mitigated, allowing the program to terminate naturally in such rare events.

## Testing Blueprint

A multi-tiered testing strategy is employed:

### Unit Testing

The `indexer` component serves as the primary test unit, with `indextest` providing a mechanism to validate the indexing results.

### System Testing

A comprehensive testing script, `testing.sh`, is devised to simulate a variety of operational scenarios, including invalid inputs and permission issues. This script, executed via `bash -v testing.sh`, intersperses command executions with their corresponding outputs for thorough examination.

### Test Execution

- **Fault Injection**: Simulated through invocations with incorrect arguments or permissions, and tests against non-crawler directories.
- **Memory Leak Checks**: Conducted using Valgrind to ensure clean memory usage.
- **Output Verification**: Involves manual inspection of generated files and directories to confirm accurate indexing behavior.

This structured approach facilitates a detailed assessment of the indexer's functionality and resilience, ensuring robust performance in line with project specifications.
