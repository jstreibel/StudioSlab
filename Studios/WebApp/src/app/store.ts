import { configureStore } from '@reduxjs/toolkit'
import workbenchReducer from '../features/workbench/workbenchSlice'

export const store = configureStore({
  reducer: {
    workbench: workbenchReducer,
  },
})

export type RootState = ReturnType<typeof store.getState>
export type AppDispatch = typeof store.dispatch
