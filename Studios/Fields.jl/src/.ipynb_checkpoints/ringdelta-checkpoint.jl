using CUDA

export δ_ring!

function δₖₑᵣₙ!(δ::CuDeviceMatrix{Float64, 1}, t::Float64, ε::Float64, N::Int64, xₘᵢₙ::Float64, h::Float64, α::Float64)
    i, j = (blockIdx().x - 1) * blockDim().x + threadIdx().x, (blockIdx().y - 1) * blockDim().y + threadIdx().y
    
    idx = i + (j-1)*N
    
    x = xₘᵢₙ + i*h
    y = xₘᵢₙ + j*h
    
    arg = sqrt(x^2+y^2)-t
    
    # 1. / sqrt(4 * M_PI * eps) * exp(-x * x / (4 * eps));
    
    δ[idx] = α * exp(-arg*arg/(4ε))/sqrt(4π*ε)
    
    @static if false
        abs_arg = abs(arg)
        ε⁻¹=1/ε    
        
        @inbounds begin
             δ[idx] = α*(abs_arg < ε)*(1 - abs_arg*ε⁻¹)*ε⁻¹
        end 
    end

    return
end

function δ_ring!(out, t, ε, N, xₘᵢₙ, h, α=1.0)
    block_size = (32, 32, 1)
    n = Integer(N/32)
    grid_size = (n, n, 1)

    @cuda threads=block_size blocks=grid_size δₖₑᵣₙ!(out, t, ε, N, xₘᵢₙ, h, α)

    
    return out
end

function δ_ring_no_kernel!(out, R, t, ε, N, xₘᵢₙ, h)

    absarg = abs.(R .- t)
        
    out = (absarg .< ε) * (1 .- absarg./ε)
    
    return
end

function TestRingDelta()
    xₘᵢₙ = x₀[1]

    R = CuArray{Float64}([√(x^2+y^2) for x in xₘᵢₙ:h:xₘᵢₙ+L-h, y in xₘᵢₙ:h:xₘᵢₙ+L-h])
    δ = CuArray{Float64}(undef, N, N)

    N²=N^2
    println("N  = $N\nN² = $N²")
    
    t = 0.25L
    ε = 0.1*L

    δ_ring!(δ, t, ε, N, xₘᵢₙ, h)

    X = repeat(xₘᵢₙ:h:xₘᵢₙ+L, 1, N)
    Y = repeat((xₘᵢₙ:h:xₘᵢₙ+L)', N, 1)
    Z = Array(δ)
    
    if false
        #using Plots
        plotlyjs()

        p = plot()
        surface!(p, X, Y, Z, title = "ring δ", size=(800,800))
    end
    
    if false
        #using GLMakie

        t = 0.5
        ε = 4.5

        fig = Figure()
        ax = Axis3(fig[1,1])
        surface!(ax, X, Y, Z)

        fig
    end
    
end