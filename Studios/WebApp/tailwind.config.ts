import type { Config } from 'tailwindcss'

const config: Config = {
  content: ['./index.html', './src/**/*.{ts,tsx}'],
  theme: {
    extend: {
      colors: {
        slab: {
          ink: '#081019',
          deep: '#112235',
          panel: '#172c42',
          line: '#2b4b66',
          cyan: '#8ad8eb',
          gold: '#e6ab63',
          lime: '#8fde86',
          rose: '#f08d7a',
          cream: '#f7efe3',
        },
      },
      fontFamily: {
        display: ['"Space Grotesk"', '"Avenir Next"', '"Segoe UI"', 'sans-serif'],
        sans: ['"Satoshi"', '"Avenir Next"', '"Segoe UI"', 'sans-serif'],
        mono: ['"IBM Plex Mono"', '"SFMono-Regular"', '"Consolas"', 'monospace'],
      },
      boxShadow: {
        panel: '0 24px 70px rgba(3, 7, 18, 0.44)',
        ember: '0 0 0 1px rgba(230, 171, 99, 0.22), 0 20px 50px rgba(230, 171, 99, 0.14)',
      },
    },
  },
  plugins: [],
}

export default config
