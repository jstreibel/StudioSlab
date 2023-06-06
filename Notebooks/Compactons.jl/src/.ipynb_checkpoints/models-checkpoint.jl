using LoopVectorization

export Model
export fieldeq!, gethamiltonian, getenergy
export signumgordon, quadratic, toy, generalizedmodel

struct Model
    V::Function
    V′::Function
end

signumgordon = Model(
    φ -> abs(φ),
    φ -> sign(φ)
)

quadratic = Model(
    η -> abs(mod(η, 2)) - mod(η, 2)^2 / 2,
    η -> sign(mod(η, 2)) - mod(η, 2)
)

toy = Model(
    η -> abs(mod(η - 1, 2) - 1),
    η -> sign(mod(η - 1, 2) - sign(mod(η - 1, 2)))
)

generalizedmodel(k) = Model(
    η -> (2 - k) / 2 * (1 - abs(mod(η, 2) - 1)^(k + 1)),
    η -> -(2 - k) / 2 * (k + 1) * abs(mod(η, 2) - 1)^k * sign(mod(η, 2) - 1)
)

function fieldeq!(∂ₜₜφ, ∂ₜφ, φ, (model, N, dx), t)
    ∂ₜₜφ[1] = 0
    @tturbo for i ∈ 2:N-1
        ∂ₜₜφ[i] = (φ[i+1] + φ[i-1] - 2φ[i]) / dx^2 - model.V′(φ[i])
    end
    ∂ₜₜφ[N] = 0

    nothing
end

𝒯(∂ₜφ, ∂ₓφ) = (∂ₜφ^2 + ∂ₓφ^2) / 2

function gethamiltonian(u, t, integrator)
    model, N, dx = integrator.p
    save_idxs = integrator.opts.save_idxs .- N

    φ = @views u[N+1:2N]
    ∂ₜφ = @views u[1:N]

    H = zero(φ)
    for i ∈ intersect(2:N-1, save_idxs)
        @inbounds H[i] = 𝒯(∂ₜφ[i], (φ[i+1] - φ[i-1]) / (2dx)) + model.V(φ[i])
    end

    return H[save_idxs]
end

function getenergy(u, t, integrator)
    model, N, dx = integrator.p

    φ = @views u[N+1:2N]
    ∂ₜφ = @views u[1:N]

    E = 0.0
    for i ∈ 2:N-1
        @inbounds E += dx * (𝒯(∂ₜφ[i], (φ[i+1] - φ[i-1]) / (2dx)) + model.V(φ[i]))
    end

    return E
end
