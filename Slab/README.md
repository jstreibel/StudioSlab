# Slab (Root-Level Platform Layer)

This folder is the landing zone for the new shared platform/framework architecture as V2 boundaries are migrated out of legacy locations.

Current intent:
- `Slab/` holds shared runtime/data/monitor infrastructure
- `Studios/` holds executable studies/apps
- `Lib/` remains the legacy/core codebase and still contains model/domain implementations during migration

See `Docs/slab-root-migration-policy.md` for migration scope and rules.
