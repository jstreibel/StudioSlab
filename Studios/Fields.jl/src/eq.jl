import CUDA

export signumGordon!, gethamiltonian, getenergy


function signumGordon!(uₜ, u, (N, dx, ∇²ϕ), t)
    ϕ  = @views u[1:N]
    ϕₜ = @views u[N+1:2N]

    compute_∇²_1d!(∇²ϕ, ϕ, dx)

    # Eq. system
    # φ ≡ ϕₜ
    φ  = ϕₜ
    φₜ = ∇²ϕ - sign.(ϕ)

    # Output
    uₜ[1:N] = φ
    uₜ[N+1:2N] = φₜ

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