import { createSlice, type PayloadAction } from '@reduxjs/toolkit'

export type SurfaceId = 'workspace' | 'runtime' | 'atlas'
export type LaunchTarget = 'desktop' | 'wasm' | 'webapp'
export type LayoutDensity = 'compact' | 'balanced'

export const surfaceOptions: Array<{
  id: SurfaceId
  label: string
  eyebrow: string
  summary: string
  accentClass: string
}> = [
  {
    id: 'workspace',
    label: 'Workspace Shell',
    eyebrow: 'Docked orchestration',
    summary: 'Editor-grade panels, launch strips, and browser-safe session surfaces.',
    accentClass: 'from-slab-cyan/30 via-slab-cyan/12 to-transparent',
  },
  {
    id: 'runtime',
    label: 'Runtime Console',
    eyebrow: 'Task posture',
    summary: 'Simulation state, event rails, and live-control handoff for bounded web sessions.',
    accentClass: 'from-slab-gold/28 via-slab-gold/10 to-transparent',
  },
  {
    id: 'atlas',
    label: 'Study Atlas',
    eyebrow: 'Docs and presets',
    summary: 'Specs, presets, and reproducibility notes collected into one browser surface.',
    accentClass: 'from-slab-lime/24 via-slab-lime/10 to-transparent',
  },
]

export const launchTargetOptions: Array<{
  id: LaunchTarget
  label: string
  detail: string
}> = [
  { id: 'desktop', label: 'Desktop', detail: 'Bridge into the existing LabV2 workbench.' },
  { id: 'wasm', label: 'WASM', detail: 'Host bounded browser experiments and runtime spikes.' },
  { id: 'webapp', label: 'WebApp', detail: 'Build a native web shell around StudioSlab workflows.' },
]

export const densityOptions: Array<{
  id: LayoutDensity
  label: string
}> = [
  { id: 'compact', label: 'Compact' },
  { id: 'balanced', label: 'Balanced' },
]

type WorkbenchState = {
  activeSurfaceId: SurfaceId
  launchTarget: LaunchTarget
  layoutDensity: LayoutDensity
  syncTelemetry: boolean
  pinEventRail: boolean
  exposeOperatorNotes: boolean
  historyWindow: number
}

const initialState: WorkbenchState = {
  activeSurfaceId: 'workspace',
  launchTarget: 'webapp',
  layoutDensity: 'balanced',
  syncTelemetry: true,
  pinEventRail: true,
  exposeOperatorNotes: false,
  historyWindow: 240,
}

const workbenchSlice = createSlice({
  name: 'workbench',
  initialState,
  reducers: {
    setActiveSurfaceId(state, action: PayloadAction<SurfaceId>) {
      state.activeSurfaceId = action.payload
    },
    setLaunchTarget(state, action: PayloadAction<LaunchTarget>) {
      state.launchTarget = action.payload
    },
    setLayoutDensity(state, action: PayloadAction<LayoutDensity>) {
      state.layoutDensity = action.payload
    },
    setHistoryWindow(state, action: PayloadAction<number>) {
      state.historyWindow = action.payload
    },
    toggleSyncTelemetry(state) {
      state.syncTelemetry = !state.syncTelemetry
    },
    togglePinEventRail(state) {
      state.pinEventRail = !state.pinEventRail
    },
    toggleExposeOperatorNotes(state) {
      state.exposeOperatorNotes = !state.exposeOperatorNotes
    },
  },
})

export const {
  setActiveSurfaceId,
  setHistoryWindow,
  setLaunchTarget,
  setLayoutDensity,
  toggleExposeOperatorNotes,
  togglePinEventRail,
  toggleSyncTelemetry,
} = workbenchSlice.actions

export default workbenchSlice.reducer
