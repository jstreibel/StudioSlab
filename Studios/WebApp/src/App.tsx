import './App.css'

function App() {
  const isingHref = `${import.meta.env.BASE_URL}wasm/ising-workspace-wasm-sandbox.html`
  const githubHref = 'https://github.com/jstreibel/StudioSlab'

  return (
    <main className="landing">
      <section className="hero-card">
        <div className="hero-card__inner">
          <h1 className="hero-card__title">Studio Slab</h1>
          <p className="hero-card__description">A little collection of programmatic studies</p>
        </div>
      </section>

      <div className="launch-stack">
        <a className="launch-card" href={isingHref} target="_blank" rel="noreferrer">
          <div>
            <p className="launch-card__eyebrow">Study</p>
            <h2 className="launch-card__title">Ising Model</h2>
            <p className="launch-card__description">Open the browser-hosted Ising workspace sandbox.</p>
          </div>
          <span className="launch-card__action">Open</span>
        </a>

        <a className="launch-card" href={githubHref} target="_blank" rel="noreferrer">
          <div>
            <p className="launch-card__eyebrow">Source</p>
            <h2 className="launch-card__title">GitHub</h2>
            <p className="launch-card__description">Browse the Studio Slab repository and project history.</p>
          </div>
          <span className="launch-card__action">Visit</span>
        </a>
      </div>
    </main>
  )
}

export default App
