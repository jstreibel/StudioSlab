using DifferentialEquations, CUDA
using Fields

include("src/laplacian.jl")


# Parameters
L = 2;
N = 128;
tmax = 1.125;

dx = L/(N-1);

x = 0.0:dx:L;
x₀ = L/2;


# Initial conditions
φ₀   = zero(x);
∂ₜφ₀ = ∂ₜoscillon.(0, x .- x₀);
uₕₒₛₜ = cat(φ₀, ∂ₜφ₀; dims=1);
u₀ = CuArray(uₕₒₛₜ);

∇²ϕ  = CuArray(zero(x));


# Equation
function fieldeq!(uₜ, u, (N, dx, ∇²ϕ), t)
     # φ  = ϕₜ
     # φₜ = ∇²ϕ - sign(ϕ)
     
     ϕ  = @views u[1:N]
     ϕₜ = @views u[N+1:2N]

     compute_laplacian!(∇²ϕ, ϕ, dx, N)

     φ  = ϕₜ
     φₜ = ∇²ϕ - sign.(ϕ)

     uₜ[1:N] = φ
     uₜ[N+1:2N] = φₜ
 
     nothing
end


# Problem
prob = ODEProblem(fieldeq!, u₀, (0.0, tmax), (N, dx, ∇²ϕ))


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