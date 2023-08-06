using DifferentialEquations, CUDA
using Fields


# Parameters
L = 2;
N = 128;
tmax = 1.125;

dx = L/(N-1);

x = 0.0:dx:L;
t = 0.0:dx/10:tmax
x₀ = L/2;


# Initial conditions
φ₀   = zero(x);
∂ₜφ₀ = ∂ₜoscillon.(0, x .- x₀);
uₕₒₛₜ = cat(φ₀, ∂ₜφ₀; dims=1);
u₀ = CuArray(uₕₒₛₜ);

∇²ϕ  = CuArray(zero(x));


# Problem
prob = ODEProblem(signumGordon!, u₀, (0.0, tmax), (N, dx, ∇²ϕ))


# Solve
@time sol = solve(prob, RK4(); adaptive=false, dt=dx / 10)

# Organize solution
copyto!(uₕₒₛₜ, sol.u[end])
uₗₐₛₜ = reduce(hcat, uₕₒₛₜ)
φ  = transpose(uₗₐₛₜ[1:N])
φₜ = transpose(uₗₐₛₜ[N+1:2N])

M = size(sol.u)[1]
uₐₗₗ = [zeros(Float64, 2N) for _ in 1:M]
copyto!(uₐₗₗ, sol.u)
uₐₗₗ = transpose(reduce(hcat, uₐₗₗ))
ϕ  = uₐₗₗ[:, 1:end÷2]
ϕₜ = uₐₗₗ[:, (end÷2+1):end]


# Plot
using Plots; gr()
heatmap(ϕ, colormap=:BrBG_11)
plot(ϕₜ,st=:surface;)
plot(ϕ[250,:])