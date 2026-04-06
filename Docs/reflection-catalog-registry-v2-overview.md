# Reflection Catalog Registry V2: Overview

## Goal

Expose one central discovery point for all reflection catalogs, then let LabV2 Schemes consume one merged catalog instead of hard-coding legacy-only access.

## Core Types

- `FReflectionCatalogSourceBindingV2`
  - source metadata (`SourceId`, `DisplayName`, `Description`)
  - callbacks (`Refresh`, `GetCatalog`, `Invoke`, `EncodeCurrentParameterValue`)
- `FReflectionCatalogRegistryV2`
  - source lifecycle (`RegisterSource`, `UnregisterSource`)
  - refresh (`RefreshAll`, `RefreshSource`)
  - discovery (`ListSources`, `BuildMergedCatalog`, `FindSourceForInterface`)
  - routing (`Invoke`, `EncodeCurrentParameterValue`)

## What Schemes Uses

- `RefreshAll()` each draw.
- `BuildMergedCatalog()` for interface/operation/parameter listing.
- `Invoke(interfaceId, operationId, ...)` for operation execution.
- `EncodeCurrentParameterValue(interfaceId, parameterId)` for live-value reads.
