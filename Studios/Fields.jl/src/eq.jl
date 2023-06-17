import CUDA

export fieldeq!, gethamiltonian, getenergy

include("laplacian.jl")

function ∇²(ϕ, dx, ∇²ϕ)
    for i in 2:length(x)-1
        ∇²ϕ[i] = (ϕ[i+1] + ϕ[i-1] - 2ϕ[i]) / dx^2
    end
end

function fieldeq!(φₜₜ, φₜ, φ, (N, dx), t)
    # ∇².(φ, dx, @views ∇²φ)

    # φₜₜ = ∇²φ - sign.(φ)
    
    # φₜₜ = .- sign.(φ)

    nothing
end


𝒯(φₜ, φₓ) = (φₜ^2 + φₓ^2) / 2

function gethamiltonian(u, t, integrator)
    N, dx = integrator.p

    φ  = @views u[N+1:2N]
    φₜ = @views u[1:N]

    H = zero(φ)
    for i ∈ 2:N-1
        @inbounds H[i] = 𝒯(φₜ[i], (φ[i+1] - φ[i-1]) / (2dx)) + abs(φ[i])
    end

    return H[save_idxs]
end

function getenergy(u, t, integrator)
    N, dx = integrator.p

    φ = @views u[N+1:2N]
    φₜ = @views u[1:N]

    E = 0.0
    for i ∈ 2:N-1
        @inbounds E += dx * (𝒯(φₜ[i], (φ[i+1] - φ[i-1]) / (2dx)) + abs(φ[i]))
    end

    return E
end