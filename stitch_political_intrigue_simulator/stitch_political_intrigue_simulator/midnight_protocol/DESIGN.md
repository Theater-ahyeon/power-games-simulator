```markdown
# Design System Strategy: The Sovereign Intelligence

## 1. Overview & Creative North Star
The Creative North Star for this design system is **"The Sovereign Intelligence."** 

We are moving away from the "friendly" consumer-web experience. This system must evoke the cold, calculated environment of a high-stakes war room where the fate of nations is decided. It is a digital manifestation of power—precise, authoritative, and impenetrable. 

To break the "template" look, we utilize **Architectural Asymmetry**. Instead of balanced, centered layouts, use weighted sidebars and modular data clusters that feel like a custom intelligence feed. We leverage high-contrast typography scales—juxtaposing the intellectual heritage of a serif typeface with the technical precision of a geometric sans-serif—to create a "High-End Editorial" feel that commands respect.

## 2. Colors & Tonal Depth
The palette is built on a foundation of `surface` (#111316) and `surface_container_lowest` (#0c0e11) to create a void-like depth.

*   **The "No-Line" Rule:** Direct structural separation must be achieved through color shifts, not borders. Prohibit 1px solid borders for sectioning. Use `surface_container_low` against a `surface` background to define regions. 
*   **Surface Hierarchy:** Use the container tiers to "nest" importance. 
    *   Deepest Level: `surface_container_lowest` for the primary workspace.
    *   Mid Level: `surface_container` for persistent sidebars.
    *   Highest Level: `surface_container_highest` for active intelligence modules or "Pop-in" dossiers.
*   **The "Glass & Gradient" Rule:** Floating UI elements (like a diplomatic message or a threat alert) should use a custom glass effect. Use `surface_container_high` at 60% opacity with a 20px backdrop blur. 
*   **Signature Textures:** Apply a subtle linear gradient to main CTAs using `primary` (#00daf3) transitioning into `on_primary_container` (#008e9f) at a 45-degree angle. This adds a "lithographic" sheen that feels expensive and technical.

## 3. Typography: The Power Dialectic
We employ a dual-font strategy to represent the two sides of political intrigue: the historical weight of the office and the cold reality of the data.

*   **Editorial Authority (Serif):** Use **Newsreader** for `display` and `headline` scales. This provides a sense of prestige and "Deep State" heritage. It should feel like a redacted document or a high-end broadsheet.
*   **Technical Precision (Sans-Serif):** Use **Inter** for `body` and `title` scales to ensure maximum legibility of complex political text.
*   **The Intelligence Layer:** Use **Space Grotesk** for `label-md` and `label-sm`. The slightly wider, technical stance of Space Grotesk is reserved for data points, coordinates, and timestamps. It mimics a head-up display (HUD).

## 4. Elevation & Depth: Tonal Layering
In a 0px radius environment, shadows and layers are the only way to prevent the UI from feeling "flat" or "dead."

*   **The Layering Principle:** Depth is achieved by stacking. Place a `surface_container_highest` card on top of a `surface_container_low` background to create a hard-edged, physical "lift."
*   **Ambient Shadows:** When an element must "float" (e.g., a high-priority tactical map), use a massive, diffused shadow: `box-shadow: 0 20px 50px rgba(0, 0, 0, 0.5)`. The shadow should feel like ambient occlusion, not a drop shadow.
*   **The "Ghost Border" Fallback:** If a layout becomes too muddy, use a "Ghost Border": `outline_variant` (#45464d) at 15% opacity. This defines the edge without creating a visual "cage."
*   **Hard Edges:** All elements must strictly follow the **0px roundedness scale**. Every corner is a sharp right angle. This reinforces the "War Room" aesthetic—there is no room for softness in political intrigue.

## 5. Components

### Buttons: The Executive Action
*   **Primary:** Background: `primary` (#00daf3). Text: `on_primary` (#00363d). Shape: Sharp 0px. Use a subtle 1px internal "inner glow" gradient to make it look like a backlit physical button.
*   **Secondary (Prestige):** Background: Transparent. Border: 1px `secondary` (#e9c349). Text: `secondary`. Used for high-level diplomatic maneuvers.
*   **Tertiary (Conflict):** Background: Transparent. Text: `tertiary` (#ffb4ac). Used for "Declare War" or "Espionage" actions.

### Input Fields: The Dossier Entry
*   **Style:** Minimalist. No background. Only a bottom-border using `outline_variant`. 
*   **States:** On focus, the bottom border transitions to `primary` (#00daf3) with a subtle `primary_container` glow behind the text. Use `label-sm` (Space Grotesk) for the persistent floating label.

### Cards & Intelligence Modules
*   **Constraint:** Forbid divider lines within cards.
*   **Separation:** Use `surface_container_lowest` for the card body and `surface_container_high` for the card header. The shift in tone creates a clear distinction between metadata and content without cluttering the view.

### Progress Bars: The Tension Meter
*   **Execution:** Background: `surface_container_highest`. Fill: A gradient from `secondary` (#e9c349) to `tertiary` (#ffb4ac) to visualize the shift from stability to crisis.

## 6. Do's and Don'ts

### Do:
*   **Embrace Negative Space:** Allow large gutters between modules. It makes the data that *is* there feel more significant.
*   **Use Mono-spacing for Numbers:** Ensure all currency or political points use `Space Grotesk` to prevent layout shifting during real-time updates.
*   **Intentional Color:** Only use `secondary` (Gold) for "Prestige" items. Only use `tertiary` (Crimson) for "Threats." If everything is colored, nothing is important.

### Don't:
*   **No Rounded Corners:** Ever. Even a 2px radius breaks the "Intelligence Agency" immersion.
*   **No Standard Shadows:** Avoid the "Material Design" look. If it looks like a standard Android app, you have failed the brief.
*   **No Scrolling Lists with Dividers:** Use vertical spacing and background tonal shifts (`surface_container_low` vs `surface_container_lowest`) to separate list items. Dividers are for spreadsheets; this is a war room.
*   **No Centered Text:** Political data is read, not admired. Align almost all text to the left to maintain an authoritative, document-style rhythm.```