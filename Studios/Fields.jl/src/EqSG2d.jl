
export signumGordon_δ_2d!, signumGordon_2d!

function signumGordon_δ_2d!(uₜ::CuArray{Float64, 3}, u::CuArray{Float64, 3}, (N, h, ∇²ϕ, δ, ε, xₘᵢₙ, α), t)
    ϕ  = @views u[:,:,1]
    ϕₜ = @views u[:,:,2]
        
    compute_∇²_2d!(∇²ϕ, ϕ, h)
    δ = δ_ring!(δ, t, ε, N, xₘᵢₙ, h, α)
    
    # Eq. system
    # φ ≡ ϕₜ
    # φₜ ≡ ϕₜₜ
    φ  = ϕₜ
    φₜ = ∇²ϕ - sign.(ϕ) + δ

    # Output
    uₜ[:,:,1] = φ
    uₜ[:,:,2] = φₜ

    nothing
end


function signumGordon_2d!(uₜ, u, (N, h, ∇²ϕ), t)
    ϕ  = @views u[:,:,1]
    ϕₜ = @views u[:,:,2]

    compute_∇²_2d!(∇²ϕ, ϕ, h)

    # Eq. system
    # φ ≡ ϕₜ
    # φₜ ≡ ϕₜₜ
    φ  = ϕₜ
    φₜ = ∇²ϕ - sign.(ϕ)

    # Output
    uₜ[:,:,1] = φ
    uₜ[:,:,2] = φₜ

    nothing
end