import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import tailwindcss from '@tailwindcss/vite'

export default defineConfig({
  plugins: [react(), tailwindcss()],
  server: {
    // no "npm run dev" o /api vai pro servidor C++ (127.0.0.1 pq o C++ so escuta no IPv4)
    proxy: {
      '/api': 'http://127.0.0.1:8080',
    },
  },
})
