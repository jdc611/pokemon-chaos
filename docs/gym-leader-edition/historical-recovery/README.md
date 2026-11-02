# Gym Leader Edition — Historical Recovery Package

Prepared November 1, 2026 (America/Chicago). Documentation only on `gym-leader-edition`. No gameplay code, existing assets, master, or other branches modified.

## Recovery result

The supplied **Choose Rebuild Town** share recovered 856 original visible turns, including 422 user messages. All 18 latest type lineups and 72 named species/location slots are present, including the later Regi/group-quest replacements. **One slot is unresolved**: Articuno still appears in Ice while all six birds were approved for universal research. No substitute was invented.

The five voucher categories and their mechanics are recovered. Exact 90 type/category species lists were not enumerated in this source; Work mode was explicitly expected to build them. The database leaves missing membership null. Thirteen acquisition-related research records, two ability-upgrade records, and one grouped conditional form-research record preserve known species and proposed stages without inventing NPC names or map coordinates.

The old `../DECISION-LEDGER.md` is preserved as historical secondary evidence. This package corrects its incomplete recovery and identified misinterpretations using the original exchanges; it does not overwrite it.

## Documents

1. [Master Design Specification](01-Master-Design-Specification.md)
2. [Complete Legendary and Pokémon Allocation Database](02-Legendary-and-Pokemon-Allocation-Database.md)
3. [Voucher Pool Database](03-Voucher-Pool-Database.md)
4. [Legendary Research Quest Database](04-Legendary-Research-Quest-Database.md)
5. [Honor and NPC Interaction Specification](05-Honor-and-NPC-Interactions.md)
6. [Recruitment and Rival Systems Specification](06-Recruitment-and-Rival.md)
7. [Competition and Tournament Specification](07-Competitions-and-Tournaments.md)
8. [Town Development and Economy Specification](08-Town-Development-and-Economy.md)
9. [Complete Historical Decision Ledger](09-Complete-Historical-Decision-Ledger.md)
10. [Missing Decisions and Conflicts Report](10-Missing-Decisions-and-Conflicts.md)
11. [Special Pokémon and Assets](11-Special-Pokemon-and-Assets.md)
12. [Repository and Source Audit](12-Repository-and-Source-Audit.md)
13. [Type Identity and Pool Catalog](13-Type-Identity-and-Pool-Catalog.md)

## Evidence and use

Read the [conflict report](10-Missing-Decisions-and-Conflicts.md) before implementation. Each system row links to original source turns. The historical ledger preserves every user answer with its preceding assistant context; all assistant turns remain in the transcript and JSON ledger. APPROVED, PROPOSED, DEFERRED, UNRESOLVED, UNRECOVERED have distinct meanings. Exact short answers are never interpreted without the prior question.

JSON/CSV databases under `data/` support allocation checks. `allocation-audit.json` reports the known Articuno collision and every still-unrecovered source pool. Complete all-species coverage is explicitly false until actual voucher/wild/Game Corner/gift/postgame pools are recovered or authored under separate implementation authority. No game build or gameplay test is claimed.

## Decisions requiring confirmation

Resolve Articuno placement, legendary failure/retry behavior, demotion with already-trained overcap Pokémon, Slaking initial/upgrade ability sequence, and whether some recruit departures may be permanent. Numerical tuning already delegated stays delegated. Full evidence and other supersessions are in report 10.
